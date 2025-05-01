#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "buffer.h"
#include "cat.h"
#include "info.h"
#include "pins.h"
#include "rig_uart.h"
#include "rig_commands.h"

#define TAG "RIG_UART"

#define RECV_OK 0
#define RECV_OVERFLOW 1
#define RECV_BUFFER_FULL 2

typedef struct {
    int status;
    char data[RECV_BUFFER_SIZE];
    size_t len;
} input_data_t;

static QueueHandle_t uart_event_queue;
static QueueHandle_t input_queue;
static buffer_t *buffer = NULL;

// UART interrupt handler task
static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    info_t *info = get_info();
    input_data_t input_data;

    while (1) {
        if (xQueueReceive(uart_event_queue, (void *)&event, portMAX_DELAY)) {
            switch (event.type) {
            case UART_DATA:
                if (event.size > RECV_BUFFER_SIZE) {
                    ESP_LOGE(TAG, "Received data length exceeds buffer size");
                    break;
                }
                char data[RECV_BUFFER_SIZE];
                int len = uart_read_bytes(UART_NUM, data, event.size, portMAX_DELAY);
                if (len > 0) {
                    if (len > info->uart_max_read_len) {
                        info->uart_max_read_len = len;
                    }
                    
                    input_data.status = RECV_OK;
                    memcpy(input_data.data, data, len);
                    input_data.len = len;
                    input_data.data[len] = '\0';
                    if (xQueueSend(input_queue, &input_data, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
                        ESP_LOGE(TAG, "Input queue overflow");
                        info->input_queue_full++;
                    }
                }
                break;

            case UART_BREAK:
                ESP_LOGI(TAG, "UART break signal detected");
                break;

            case UART_BUFFER_FULL:
                ESP_LOGW(TAG, "UART buffer full");
                info->uart_buffer_full++;

                input_data.status = RECV_BUFFER_FULL;
                input_data.len = 0;
                if (xQueueSend(input_queue, &input_data, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
                    ESP_LOGE(TAG, "Input queue overflow");
                    info->input_queue_full++;
                }
                break;

            case UART_FIFO_OVF:
                ESP_LOGW(TAG, "UART FIFO overflow");
                info->uart_fifo_ovf++;

                input_data.status = RECV_BUFFER_FULL;
                input_data.len = 0;
                if (xQueueSend(input_queue, &input_data, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
                    ESP_LOGE(TAG, "Input queue overflow");
                    info->input_queue_full++;
                }
                break;

            case UART_FRAME_ERR:
                ESP_LOGE(TAG, "UART frame error");
                break;

            case UART_PARITY_ERR:
                ESP_LOGE(TAG, "UART parity error");
                break;

            case UART_DATA_BREAK:
                ESP_LOGI(TAG, "UART data and break signal detected");
                break;

            case UART_PATTERN_DET: 
                ESP_LOGI(TAG, "UART pattern detected");
                break;

            case UART_WAKEUP:
                ESP_LOGI(TAG, "UART wakeup event");
                break;

            default:
                ESP_LOGW(TAG, "Unhandled UART event type: %d", event.type);
                break;
            }
        }
    }

    vTaskDelete(NULL);
}

static int buffer_fill_callback(void *context, uint8_t *buf, size_t max_len) {
    int retry_count = 0;

    while (retry_count < 3) {
        input_data_t input_data;

        if (xQueueReceive(input_queue, &input_data, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) == pdPASS) {
            switch(input_data.status) {
            case RECV_OK:
                memccpy((char *)buf, input_data.data, input_data.len, max_len);
                return input_data.len;

            case RECV_OVERFLOW:
            case RECV_BUFFER_FULL:
                ESP_LOGW(TAG, "Buffer overflow or full");
                return -1;
            }
        }
        retry_count++;

        ESP_LOGW(TAG, "Timeout while reading response, retry: %d", retry_count);
    }

    return 0;
}

int rig_uart_send(char *data, size_t len) {
    int ret = uart_write_bytes(UART_NUM, data, len);
    uart_wait_tx_done(UART_NUM, pdMS_TO_TICKS(1000));
    if (ret < 0) {
        ESP_LOGE(TAG, "Failed waiting for TX done");
        return -1;
    }
    return ret;
}

int rig_uart_recv_len() {
    return buffer_len(buffer);
}
int rig_uart_recv_byte(char *data) {
    return buffer_get_byte(buffer, (uint8_t *)data);
}
void rig_uart_clear_buffer() {
    buffer_flush(buffer);
}

int rig_uart_peek_buffer(char *data, size_t len) {
    int ret = buffer_peek(buffer, (uint8_t *)data, len);
    if (ret < 0) {
        ESP_LOGW(TAG, "Failed to read from buffer");
        return -1;
    }
    return ret;
}

void rig_uart_flush() {
    if (input_queue != NULL) {
        xQueueReset(input_queue);
    }
    if (uart_event_queue != NULL) {
        xQueueReset(uart_event_queue);
    }

    uart_flush(UART_NUM);

    buffer_flush(buffer);
}

// Initialize the UART driver with interrupt-based reading
esp_err_t rig_uart_init(void) {
    // Create the data queue
    input_queue = xQueueCreate(INPUT_QUEUE_SIZE, sizeof(input_data_t));
    if (input_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create data queue");
        return ESP_FAIL;
    }

    // Initialize the UART driver
    
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    if (uart_param_config(UART_NUM, &uart_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure UART parameters");
        return ESP_FAIL;
    }

    if (uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set UART pins");
        return ESP_FAIL;
    }

    // Install UART driver with RX buffer and event queue
    if (uart_driver_install(UART_NUM, 129, 129, 20, &uart_event_queue, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install UART driver");
        return ESP_FAIL;
    }

    uart_flush_input(UART_NUM);

    buffer = buffer_create(RECV_BUFFER_SIZE, buffer_fill_callback, NULL);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to create buffer");
        return ESP_FAIL;
    }

    // Create a task to handle UART events
    if (xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UART event task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "UART initialized with interrupt-based reading");

    return ESP_OK;
}
