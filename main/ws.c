#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "http.h"
#include "info.h"
#include "ui.h"
#include "ws.h"
#include <esp_netif_types.h>
#include "esp_wifi.h"
#include "esp_event.h"

#define TAG "WS"
#define WS_QUEUE_SIZE 300

static QueueHandle_t ws_queue;
static const size_t max_clients = 4;

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
    const char *message;
};

static void ws_send_handler(void *context, void *data) {
    if (ws_queue == NULL) {
        ESP_LOGE(TAG, "WebSocket queue not initialized");
        return;
    }

    char *buf = malloc(strlen(data) + 1);
    if (buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for WebSocket message");
        return;
    }
    strcpy(buf, (char *)data);

    if (xQueueSend(ws_queue, &buf, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to enqueue WebSocket message");
    }
}

static void send_message(void *arg) {
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)resp_arg->message;
    ws_pkt.len = strlen(resp_arg->message);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    // if (resp_arg->message[0] != 'S' && resp_arg->message[1] != 'M' && resp_arg->message[2] != '0') {
    //     ESP_LOGI(TAG, "Sending WebSocket message: %s", resp_arg->message);
    // }

    // ESP_LOGI(TAG, "WebSocket message: %s, %d", ws_pkt.payload, ws_pkt.len);
    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free((char *)resp_arg->message);
    free(resp_arg);
}

static void ws_send_task(void *arg) {
    httpd_handle_t *server = (httpd_handle_t *)arg;
    char *message;
    info_t *info = get_info();

    while (1) {
        info->ws_queue_count = uxQueueMessagesWaiting(ws_queue);
        if (xQueueReceive(ws_queue, &message, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE(TAG, "Failed to receive WebSocket message");
            continue;
        }
        info->ws_queue_polls++;
        
        size_t clients = max_clients;
        int    client_fds[max_clients];
        if (httpd_get_client_list(*server, &clients, client_fds) == ESP_OK) {
            for (size_t i=0; i < clients; ++i) {
                int sock = client_fds[i];
                if (httpd_ws_get_fd_info(*server, sock) == HTTPD_WS_CLIENT_WEBSOCKET) {
                    // ESP_LOGI(TAG, "Active client (fd=%d) -> sending async message", sock);

                    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
                    assert(resp_arg != NULL);
                    resp_arg->hd = *server;
                    resp_arg->fd = sock;
                    resp_arg->message = message;

                    if (httpd_queue_work(resp_arg->hd, send_message, resp_arg) != ESP_OK) {
                        ESP_LOGE(TAG, "httpd_queue_work failed!");
                        break;
                    }
                }
            }
        } else {
            ESP_LOGE(TAG, "httpd_get_client_list failed!");
            return;
        }
    }
}

static esp_err_t ws_handler(httpd_req_t *req) {
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    uint8_t *buf = NULL;

    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    // ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;

        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);

        ui_recv((char *)buf);
    }
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);

    free(buf);
    return ret;
}

void ws_init() {
    ws_queue = xQueueCreate(WS_QUEUE_SIZE, sizeof(char *));
    if (ws_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create WebSocket queue");
        return;
    }

    
    static const httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = ws_handler,
        .user_ctx   = NULL,
        .is_websocket = true
    };
    
    register_handler(&ws);

    ui_add_observers(ws_send_handler, NULL);

    xTaskCreate(ws_send_task, "ws_send_task", 4096, get_http_server(), 5, NULL);
}