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
    RESULT_NO_RESPONSE,
    RESULT_TIMEOUT,
    RESULT_ERROR,
} process_command_result_t;

static QueueHandle_t send_queue = NULL;

static bool is_memory_read(response_t response) {
    if (response.read_len == 0 || response.read[0] != 'R' || response.read[1] != 'M') {
        return false;
    }
    return true;
}

static process_command_result_t process_command(response_t *response) {
    info_t *info = get_info();

    // Combine command and read into a single send buffer

    if (response->command_len + response->read_len > SEND_BUFFER_SIZE * 2) {
        ESP_LOGE(TAG, "Command and read buffer size exceeds maximum allowed size");
        response->result = RECV_RESULT_ERROR;
        return RESULT_ERROR;
    }
    if (response->command_len == 0 && response->read_len == 0) {
        ESP_LOGE(TAG, "Both command and read buffers are empty");
        response->result = RECV_RESULT_ERROR;
        return RESULT_ERROR;
    }

    char send_buf[SEND_BUFFER_SIZE * 2];
    memset(send_buf, 0, sizeof(send_buf));
    int send_buf_len = 0;
    if (response->command_len > 0) {
        memcpy(send_buf, response->command, response->command_len);
        send_buf_len = response->command_len;
    }
    if (response->read_len > 0) {
        memcpy(send_buf + send_buf_len, response->read, response->read_len);
        send_buf_len += response->read_len;
    }

    if (response->type != SEND_TYPE_POLL) {
        ESP_LOGI(TAG, "Sending command: %s", send_buf);
    }

    int bytes_written = rig_uart_send(send_buf, send_buf_len);

    if (bytes_written != send_buf_len) {
        ESP_LOGE(TAG, "Number of bytes written does not match command size: expected %d, got %d", send_buf_len, bytes_written);
    }

    if (bytes_written > info->cat_max_send_len) {
        info->cat_max_send_len = bytes_written;
    }

    if (response->read_len == 0) {
        return RESULT_NO_RESPONSE;
    }

    size_t i = 0;
    for(;;) {
        int ret = rig_uart_recv_byte(&response->response[i]);
        if (ret == -1) {
            ESP_LOGE(TAG, "Error processing command");
            response->result = RECV_RESULT_ERROR;
            return RESULT_ERROR;
        }
        if (ret == 0) {
            return RESULT_TIMEOUT;
        }

        if (response->response[i] == ';') {
            response->result = RECV_RESULT_OK;
            response->response[i + 1] = '\0';
            response->response_len = i + 1;

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
            response->result = RECV_RESULT_OVERFLOW;
            return RESULT_ERROR;
        }
    }
}

static void cat_task(void *arg) {
    info_t *info = get_info();

    while (1) {
        int no_in_sendqueue = uxQueueMessagesWaiting(send_queue);
        if (no_in_sendqueue == 0) {
            info->cat_empty_queue++;
        } else {
            info->cat_queue_busy++;
            if (no_in_sendqueue > info->cat_max_queue_size) {
                info->cat_max_queue_size = no_in_sendqueue;
            }
        }

        // Wait for data to send
        command_t command;
        
        if (xQueueReceive(send_queue, &command, portMAX_DELAY) == pdPASS) {
            int64_t start_time = esp_timer_get_time();
            int retries = 0;

            for(;;) {
                response_t response;
                memset(&response, 0, sizeof(response_t));

                response.type = command.type;
                response.result = -1;

                memcpy(response.command, command.command, command.command_len);
                response.command_len = command.command_len;

                memcpy(response.read, command.read, command.read_len);
                response.read_len = command.read_len;

                process_command_result_t pcr = process_command(&response);

                if (pcr == RESULT_NO_RESPONSE) {
                    break;
                }

                if (pcr == RESULT_OK) {
                    // The Memory Read command is a special case.
                    // It channel nmber of the command doesn't match the channel
                    // number of the response.
                    if (is_memory_read(response) && !rc_is_fail(response.response) && memcmp(response.response, response.read, response.read_len - 1) != 0) {
                        ESP_LOGW(TAG, "Received data does not match sent command, expected: %s, got: %s", response.read, response.response);
                        rig_uart_flush();
                        ESP_LOGW(TAG, "Retrying command");
                        continue;
                    }

                    if (!rc_is_fail(response.response)) {
                        int64_t end_time = esp_timer_get_time();
                        int64_t elapsed_time = end_time - start_time;
                        info->cat_total_response_time += elapsed_time;
                        info->cat_no_responses++;
                        if (elapsed_time > info->cat_max_response_time) {
                            info->cat_max_response_time = elapsed_time;
                        }

                        if (response.response_len > info->cat_max_receive_len) {
                            info->cat_max_receive_len = response.response_len;
                        }
                    }
                    if (response.type != SEND_TYPE_POLL) {
                        // log_response(TAG, &response);
                        ESP_LOGI(TAG, "Received response: %s", response.response);
                    }
                    rm_queue_response(&response);
                    break;
                }

                if (pcr == RESULT_TIMEOUT) {
                    retries++;
                    if (retries >= 3) {
                        ESP_LOGW(TAG, "Receive timeout, giving up on command");
                        response.result = RECV_RESULT_TIMEOUT;
                        rm_queue_response(&response);

                        rig_uart_flush();
                        ESP_LOGW(TAG, "Cleared buffer after timeout");
                        break;
                    } {
                        ESP_LOGW(TAG, "Receive timeout, retrying command");
                    }
                }

                if (pcr == RESULT_ERROR) {
                    rm_queue_response(&response);

                    rig_uart_flush();
                    ESP_LOGE(TAG, "Cleared buffer after error");
                    break;
                }
            }
        }
    }
}

esp_err_t cat_queue_command(command_t *command, int priority) {
    // Make sure some space is always available in the send queue for high priority commands
    UBaseType_t a = uxQueueSpacesAvailable(send_queue);
    if ((priority == SEND_PRIORITY_HIGH && a == 0) || (priority == SEND_PRIORITY_NORMAL && a <= SEND_QUEUE_MIN)) {
        return ESP_ERR_NO_MEM;
    }

    if (priority == SEND_PRIORITY_HIGH) {
        while (xQueueSendToFront(send_queue, command, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to enqueue high priority command");
        }
    } else if (priority == SEND_PRIORITY_NORMAL) {
        while (xQueueSend(send_queue, command, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to enqueue normal priority command");
        }
    } else {
        ESP_LOGE(TAG, "Invalid priority: %d", priority);
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

    send_queue = xQueueCreate(SEND_QUEUE_SIZE, sizeof(command_t));
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

void log_response(const char *tag, response_t *response) {
    char *type_str;
    switch(response->type) {
        case SEND_TYPE_COMMAND:
            type_str = "SET";
            break;
        case SEND_TYPE_READ:
            type_str = "READ";
            break;
        case SEND_TYPE_POLL:
            type_str = "POLL";
            break;
        default:
            type_str = "UNKNOWN";
            break;
    }

    char *result_str;
    switch(response->result) {
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
    ESP_LOGI(tag, "response_t, type: %s, cmd: %s/%d, read: %s/%d, response: %s/%d, result: %s",
        type_str,
        response->command, response->command_len,
        response->read, response->read_len,
        response->response, response->response_len,
        result_str
    );
}
