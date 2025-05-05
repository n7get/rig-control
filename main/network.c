#include "network.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "string.h"
#include "buffer.h"
#include "rig.h"
#include "rig_commands.h"
#include "rig_monitor.h"
#include "rig_monitor.h" // For rig_monitor_command_buf
#include "observer.h" // For observer_add

#define KEEPALIVE_IDLE              5
#define KEEPALIVE_INTERVAL          5
#define KEEPALIVE_COUNT             3
#define NETWORK_RECV_QUEUE_SIZE     10
#define TCP_PORT 9911

typedef struct {
    int channel;
    char data[RECV_BUFFER_SIZE];
    size_t len;
} send_queue_buf_t;

static const char *TAG = "NETWORK";
static QueueHandle_t tcp_send_queue = NULL;

// Callback function to put received rig data onto the network send queue
static void tcp_send_callback(void *context, void *data) {
    result_buf_t *result = (result_buf_t *)data;

    send_queue_buf_t send_buf;
    send_buf.channel = *(int *)context;
    strncpy(send_buf.data, result->data, RECV_BUFFER_SIZE);
    send_buf.len = result->len;
    // ESP_LOGI(TAG, "Queuing TCP data, channel: %d, data: %s, len: %d", send_buf.channel, send_buf.data, send_buf.len);

    if (xQueueSend(tcp_send_queue, &send_buf, pdMS_TO_TICKS(100)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue network send data");
    }
}

// Callback function to fill the buffer with data
static int buffer_fill_callback(void *context, uint8_t *buf, size_t max_len) {
    int sock = *(int *)context;

    int len = recv(sock, buf, max_len, 0);

    // Error occurred during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        return -1;
    }

    // Connection closed
    if (len == 0) {
        ESP_LOGI(TAG, "Connection closed");
        return 0;
    }

    // Data received
    return len;
}

static void connection_established(int sock, buffer_t *buffer) {
    while (1) {
        char rx_buffer[SEND_BUFFER_SIZE];
        int i = 0;

        while (1) {
            int ret = buffer_get_byte(buffer, (uint8_t *)&rx_buffer[i]);

            // Error occurred during receiving
            if (ret < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                return;
            }

            // Connection closed
            if (ret == 0) {
                return;
            }

            // Buffer overflow
            if (i >= SEND_BUFFER_SIZE - 1) {
                ESP_LOGE(TAG, "Buffer overflow");
                return;
            }

            // Check for end of command
            if (rx_buffer[i] == ';') {
                rx_buffer[i + 1] = '\0';

                rm_queue_command(rx_buffer, SEND_TYPE_COMMAND);

                i = 0;
            }
            else {
                i++;
            }
        }
    }
}

static void wait_for_accept(int listen_sock) {
    int err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        return;
    }

    while (1) {
        ESP_LOGI(TAG, "Socket listening");
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break; // Exit loop on error
        }

        // Set connection parameters for keep-alive
        int keepalive = 1;
        int keepidle = KEEPALIVE_IDLE;
        int keepinterval = KEEPALIVE_INTERVAL;
        int keepcount = KEEPALIVE_COUNT;
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepinterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepcount, sizeof(int));

        // Set the current active socket for the send task
        ESP_LOGI(TAG, "Socket accepted");

        rig_monitor_add_observers(OBSERVE_COMMANDS|OBSERVE_UPDATES|OBSERVE_STATUS, tcp_send_callback, (void *)&sock);

        buffer_t *buffer = buffer_create(RECV_BUFFER_SIZE, buffer_fill_callback, (void *)&sock);
        if (buffer == NULL) {
            ESP_LOGE(TAG, "Failed to create buffer");
            close(sock);
            rig_monitor_remove_observers(tcp_send_callback);
            return;
        }

        connection_established(sock, buffer);

        // Connection closed or error, reset current_sock and close the socket
        shutdown(sock, 0);
        close(sock);
        buffer_free(buffer);
        rig_monitor_remove_observers(tcp_send_callback);
        ESP_LOGI(TAG, "Socket closed");
    }
}

// Task to handle incoming TCP connections and receive data
static void tcp_recv_task(void *pvParameters) {
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;
    struct sockaddr_storage dest_addr;

    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(TCP_PORT);

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", TCP_PORT);

    wait_for_accept(listen_sock);

    close(listen_sock);
    vTaskDelete(NULL);
}

// Task to send data from the queue to the connected TCP client
static void tcp_send_task(void *pvParameters) {
    while (1) {
        send_queue_buf_t send_buf;

        // Wait for data to appear in the queue
        if (xQueueReceive(tcp_send_queue, &send_buf, portMAX_DELAY) == pdPASS) {
            // ESP_LOGI(TAG, "Sending data, channel: %d, data: %s, len: %d", send_buf.channel, send_buf.data, send_buf.len);
            int written = send(send_buf.channel, send_buf.data, send_buf.len, 0);
            if (written < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }
        }
    }
}

// Initialize network tasks and queue
esp_err_t network_init(void) {
    tcp_send_queue = xQueueCreate(300, sizeof(send_queue_buf_t));
    if (tcp_send_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create network receive queue");
        return ESP_FAIL;
    }

    xTaskCreate(tcp_recv_task, "tcp_recv_task", 4096, NULL, 5, NULL);
    xTaskCreate(tcp_send_task, "tcp_send_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Network tasks initialized");
    return ESP_OK;
}
