#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "cat.h"
#include "info.h"
#include "pins.h"
#include "rig_monitor.h"

#define TAG "UART"

static QueueHandle_t uart_event_queue;
static QueueHandle_t input_queue;
static QueueHandle_t send_queue;

// UART interrupt handler task
static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    char *data = malloc(BUF_SIZE);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for data buffer");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        if (xQueueReceive(uart_event_queue, (void *)&event, portMAX_DELAY)) {
            switch (event.type) {
            case UART_DATA:
                if (event.size > BUF_SIZE) {
                    ESP_LOGE(TAG, "Received data length exceeds buffer size");
                    break;
                }
                int len = uart_read_bytes(UART_NUM, data, event.size, portMAX_DELAY);
                if (len > 0) {
                    for (int i = 0; i < len; i++) {
                        while (xQueueSend(input_queue, &data[i], pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
                            ESP_LOGE(TAG, "Input queue overflow");
                        }
                    }
                }
                break;

            case UART_FIFO_OVF:
                ESP_LOGW(TAG, "UART FIFO overflow");
                uart_flush_input(UART_NUM);
                xQueueReset(uart_event_queue);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGW(TAG, "UART buffer full");
                uart_flush_input(UART_NUM);
                xQueueReset(uart_event_queue);
                break;

            case UART_PARITY_ERR:
                ESP_LOGE(TAG, "UART parity error");
                break;

            case UART_FRAME_ERR:
                ESP_LOGE(TAG, "UART frame error");
                break;

            default:
                ESP_LOGW(TAG, "Unhandled UART event type: %d", event.type);
                break;
            }
        }
    }

    free(data);
    vTaskDelete(NULL);
}

// Task to handle sending and receiving data

static void uart_task(void *arg) {
    info_t *info = get_info();

    while (1) {
        int no_in_sendqueue = uxQueueMessagesWaiting(send_queue);
        if ( no_in_sendqueue == 0) {
            info->no_empty_sendqueue++;
        } else {
            info->no_busy_sendqueue++;
            info->no_sendqueue_waiting += no_in_sendqueue;
        }
        // ESP_LOGE(TAG, "Send queue size: %d", no_in_sendqueue);

        // Wait for data to send
        result_buf_t result_buf;
        if (xQueueReceive(send_queue, &result_buf.command_buf, portMAX_DELAY) == pdPASS) {
            info->total_sendqueue++;

            int64_t start_time = esp_timer_get_time();

            int bytes_written = uart_write_bytes(UART_NUM, result_buf.command_buf.data, result_buf.command_buf.len);

            if (bytes_written != result_buf.command_buf.len) {
                ESP_LOGE(TAG, "Number of bytes written does not match command size: expected %d, got %d", result_buf.command_buf.len, bytes_written);
            }

            if (bytes_written > info->max_send_len) {
                info->max_send_len = bytes_written;
            }

            size_t i = 0;
            while (1) {
                if (xQueueReceive(input_queue, &result_buf.data[i], pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
                    ESP_LOGE(TAG, "Timeout while reading response");
                    result_buf.result = RECV_RESULT_TIMEOUT;
                    rig_monitor_recv_data(&result_buf);
                    break;
                }
                if (result_buf.data[i] == ';') {
                    if (result_buf.data[0] != '?') {
                        int64_t end_time = esp_timer_get_time();
                        int64_t elapsed_time = end_time - start_time;
                        info->total_response_time += elapsed_time;
                        info->no_responses++;
                        if (elapsed_time > info->max_response_time) {
                            info->max_response_time = elapsed_time;
                        }

                        if (i + 1 > info->max_receive_len) {
                            info->max_receive_len = i + 1;
                        }
                    }

                    result_buf.result = RECV_RESULT_OK;
                    result_buf.data[i + 1] = '\0';
                    result_buf.len = i + 1;
                    // log_result_buf("uart_task", &result_buf);
                    rig_monitor_recv_data(&result_buf);

                    // length of input_queue
                    int queue_length = uxQueueMessagesWaiting(input_queue);
                    if (queue_length > 0) {
                        ESP_LOGW(TAG, "Data in input queue: %d", queue_length);
                        xQueueReset(input_queue);
                    }
                    break;
                }
                i++;
                if (i >= RECV_BUFFER_SIZE) {
                    ESP_LOGE(TAG, "Received data exceeds buffer size");
                    result_buf.result = RECV_RESULT_OVERFLOW;
                    rig_monitor_recv_data(&result_buf);
                    break;
                }
            }
        }
    }
}

// Initialize the UART driver with interrupt-based reading
esp_err_t cat_init(void) {
    send_queue = xQueueCreate(SEND_QUEUE_SIZE, sizeof(command_buf_t));
    if (send_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create UART send queue");
        return ESP_FAIL;
    }

    // Create the data queue
    input_queue = xQueueCreate(DATA_QUEUE_SIZE, sizeof(char));
    if (input_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create data queue");
        return ESP_FAIL;
    }

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
    if (uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart_event_queue, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install UART driver");
        return ESP_FAIL;
    }

    uart_flush_input(UART_NUM);

    // Create a task to handle UART events
    if (xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UART event task");
        return ESP_FAIL;
    }

    if (xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UART task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "UART initialized with interrupt-based reading");
    return ESP_OK;
}

esp_err_t cat_send(const char *command, int type, int priority) {
    size_t command_size = strnlen(command, SEND_BUFFER_SIZE);
    if (command_size == 0) {
        ESP_LOGE(TAG, "Command is empty");
        return ESP_FAIL;
    }
    if (command_size >= SEND_BUFFER_SIZE) {
        ESP_LOGE(TAG, "Command size %d exceeds buffer size %d", command_size, SEND_BUFFER_SIZE - 1);
        return ESP_FAIL;
    }
    if (command[command_size - 1] != ';') {
        ESP_LOGE(TAG, "Command must end with a semicolon");
        return ESP_FAIL;
    }

    command_buf_t command_buf;
    command_buf.type = type;
    strncpy(command_buf.data, command, SEND_BUFFER_SIZE);
    command_buf.len = command_size;

    // if (type == SEND_TYPE_MONITOR) {
    //     ESP_LOGI(TAG, "Sending command: %s", command_buf.data);
    // }

    if (priority == SEND_PRIORITY_HIGH) {
        while (xQueueSendToFront(send_queue, &command_buf, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to enqueue high priority command");
        }
    } else if (priority == SEND_PRIORITY_NORMAL) {
        while (xQueueSend(send_queue, &command_buf, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to enqueue normal priority command");
        }
    } else {
        ESP_LOGE(TAG, "Command: %s, Invalid priority: %d", command_buf.data, priority);
        return ESP_FAIL;
    }

    return ESP_OK;
}

void cat_clear() {
    if (input_queue != NULL) {
        xQueueReset(input_queue);
    }
    if (send_queue != NULL) {
        xQueueReset(send_queue);
    }
    if (uart_event_queue != NULL) {
        xQueueReset(uart_event_queue);
    }

    uart_flush(UART_NUM);
}

void log_command_buf(char *tag, command_buf_t *command_buf) {
    char *type_str;
    switch(command_buf->type) {
        case SEND_TYPE_COMMAND:
            type_str = "EXTERNAL";
            break;
        case SEND_TYPE_MONITOR:
            type_str = "MONITOR";
            break;
        default:
            type_str = "UNKNOWN";
            break;
    }
    ESP_LOGI(tag, "command_buf_t, type: %s, data: %s, len: %d", type_str, command_buf->data, command_buf->len);
}

void log_result_buf(char *tag, result_buf_t *result) {
    char *type_str;
    switch(result->command_buf.type) {
        case SEND_TYPE_COMMAND:
            type_str = "EXTERNAL";
            break;
        case SEND_TYPE_MONITOR:
            type_str = "MONITOR";
            break;
        default:
            type_str = "UNKNOWN";
            break;
    }
    char *result_str;
    switch(result->result) {
        case RECV_RESULT_OK:
            result_str = "OK";
            break;
        case RECV_RESULT_TIMEOUT:
            result_str = "TIMEOUT";
            break;
        case RECV_RESULT_OVERFLOW:
            result_str = "OVERFLOW";
            break;
        case RECV_RESULT_ERROR:
            result_str = "ERROR";
            break;
        default:
            result_str = "UNKNOWN";
            break;
    }
    ESP_LOGI(tag, "result_buf_t, cmd: %s, type: %s, result: %s, data: %s, len: %d", result->command_buf.data, type_str, result_str, result->data, result->len);
}
