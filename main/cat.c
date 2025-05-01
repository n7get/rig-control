#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "cat.h"
#include "info.h"
#include "rig_commands.h"
#include "rig_monitor.h"
#include "rig_uart.h"

#define TAG "CAT"

typedef enum {
    RESULT_OK = 0,
    RESULT_TIMEOUT,
    RESULT_ERROR,
} process_command_result_t;

static QueueHandle_t send_queue = NULL;

static process_command_result_t process_command(result_buf_t *result_buf) {
    info_t *info = get_info();

    int bytes_written = rig_uart_send(result_buf->command_buf.data, result_buf->command_buf.len);

    if (bytes_written != result_buf->command_buf.len) {
        ESP_LOGE(TAG, "Number of bytes written does not match command size: expected %d, got %d", result_buf->command_buf.len, bytes_written);
    }

    if (bytes_written > info->max_send_len) {
        info->max_send_len = bytes_written;
    }

    size_t i = 0;
    for(;;) {
        int ret = rig_uart_recv_byte(&result_buf->data[i]);
        if (ret == -1) {
            ESP_LOGE(TAG, "Error processing command");
            result_buf->result = RECV_RESULT_ERROR;
            return RESULT_ERROR;
        }
        if (ret == 0) {
            return RESULT_TIMEOUT;
        }

        if (result_buf->data[i] == ';') {
            result_buf->result = RECV_RESULT_OK;
            result_buf->data[i + 1] = '\0';
            result_buf->len = i + 1;

            int remain_recv_len = rig_uart_recv_len();
            if (remain_recv_len > 0) {
                ESP_LOGW(TAG, "Data in buffer: %d", remain_recv_len);

                char buf[128];
                int ret = rig_uart_peek_buffer(buf, 127);
                if (ret > 0) {
                    buf[ret] = '\0';
                    ESP_LOGW(TAG, "Data in buffer: %s", buf);
                } else {
                    ESP_LOGE(TAG, "Failed to read from buffer");
                }

                rig_uart_clear_buffer();
            }
            return RESULT_OK;
        }
        i++;

        if (i >= RECV_BUFFER_SIZE) {
            ESP_LOGE(TAG, "Received data exceeds buffer size");
            result_buf->result = RECV_RESULT_OVERFLOW;
            return RESULT_ERROR;
        }
    }
}

static void cat_task(void *arg) {
    info_t *info = get_info();

    while (1) {
        int no_in_sendqueue = uxQueueMessagesWaiting(send_queue);
        if (no_in_sendqueue == 0) {
            info->no_empty_sendqueue++;
        } else {
            info->no_busy_sendqueue++;
            info->no_sendqueue_waiting += no_in_sendqueue;
        }

        // Wait for data to send
        result_buf_t result_buf;

        if (xQueueReceive(send_queue, &result_buf.command_buf, portMAX_DELAY) == pdPASS) {
            int64_t start_time = esp_timer_get_time();
            info->total_sendqueue++;
            int retries = 0;

            for(;;) {
                result_buf.result = -1;
                result_buf.data[0] = '\0';
                result_buf.len = 0;

                process_command_result_t pcr = process_command(&result_buf);
                if (pcr == RESULT_OK) {
                    if (!rig_command_is_fail(result_buf.data) && memcmp(result_buf.data, result_buf.command_buf.data, result_buf.command_buf.len - 1) != 0) {
                        ESP_LOGW(TAG, "Received data does not match sent command, expected: %s, got: %s", result_buf.command_buf.data, result_buf.data);
                        rig_uart_flush();
                        ESP_LOGW(TAG, "Retrying command");
                        continue;
                    }

                    if (!rig_command_is_fail(result_buf.data)) {
                        int64_t end_time = esp_timer_get_time();
                        int64_t elapsed_time = end_time - start_time;
                        info->total_response_time += elapsed_time;
                        info->no_responses++;
                        if (elapsed_time > info->max_response_time) {
                            info->max_response_time = elapsed_time;
                        }

                        if (result_buf.len > info->max_receive_len) {
                            info->max_receive_len = result_buf.len;
                        }
                    }

                    rig_monitor_recv_data(&result_buf);
                    break;
                }

                if (pcr == RESULT_TIMEOUT) {
                    retries++;
                    if (retries >= 3) {
                        ESP_LOGW(TAG, "Receive timeout, giving up on command");
                        result_buf.result = RECV_RESULT_TIMEOUT;
                        rig_monitor_recv_data(&result_buf);

                        rig_uart_flush();
                        ESP_LOGW(TAG, "Cleared buffer after timeout");
                        break;
                    } {
                        ESP_LOGW(TAG, "Receive timeout, retrying command");
                    }
                }

                if (pcr == RESULT_ERROR) {
                    rig_monitor_recv_data(&result_buf);

                    rig_uart_flush();
                    ESP_LOGE(TAG, "Cleared buffer after error");
                    break;
                }
            }
        }
    }
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

    // Make sure some space is always available in the send queue for high priority commands
    UBaseType_t a = uxQueueSpacesAvailable(send_queue);
    if ((priority == SEND_PRIORITY_HIGH && a == 0) || a < SEND_QUEUE_MIN) {
        return ESP_ERR_NO_MEM;
    }

    command_buf_t command_buf;
    command_buf.type = type;
    strncpy(command_buf.data, command, SEND_BUFFER_SIZE);
    command_buf.len = command_size;

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

int cat_send_len() {
    return uxQueueMessagesWaiting(send_queue);
}

void cat_flush() {
    rig_uart_flush();
    if (send_queue != NULL) {
        xQueueReset(send_queue);
    }
}

// Initialize the UART driver with interrupt-based reading
esp_err_t cat_init(void) {
    if (rig_uart_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize UART");
        return ESP_FAIL;
    }

    send_queue = xQueueCreate(SEND_QUEUE_SIZE, sizeof(command_buf_t));
    if (send_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create UART send queue");
        return ESP_FAIL;
    }

    if (xTaskCreate(cat_task, "cat_task", 4096, NULL, 10, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UART task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "CAT interface initialized");
    return ESP_OK;
}

void log_command_buf(const char *tag, command_buf_t *command_buf) {
    char *type_str;
    switch(command_buf->type) {
        case SEND_TYPE_COMMAND:
            type_str = "COMMAND";
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

void log_result_buf(const char *tag, result_buf_t *result) {
    char *type_str;
    switch(result->command_buf.type) {
        case SEND_TYPE_COMMAND:
            type_str = "COMMAND";
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
