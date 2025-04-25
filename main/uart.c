#include "uart.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "pins.h"
#include <string.h>

#define TAG "UART"

static QueueHandle_t uart_event_queue;
static QueueHandle_t input_queue;
static QueueHandle_t send_queue;

static subject_t *recv_subject = NULL;

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
        // Wait for UART events
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
                        if (xQueueSend(input_queue, &data[i], portMAX_DELAY) != pdPASS) {
                            ESP_LOGE(TAG, "Data queue overflow");
                            break;
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
    send_data_t send_data;

    while (1) {
        // Wait for data to send
        if (xQueueReceive(send_queue, &send_data, portMAX_DELAY) == pdPASS) {
            int bytes_written = uart_write_bytes(UART_NUM, send_data.data, send_data.len);

            if (bytes_written != send_data.len) {
                ESP_LOGE(TAG, "Number of bytes written does not match command size: expected %d, got %d", send_data.len, bytes_written);
            }

            // Wait for response
            // FIXME: read until input_queue is empty?
            recv_result_t recv_result;
            recv_result.type = send_data.type;
            strcpy(recv_result.command, send_data.data);
            size_t i = 0;
            while (1) {
                if (xQueueReceive(input_queue, &recv_result.data[i], pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
                    ESP_LOGE(TAG, "Timeout while reading response");
                    strcpy(recv_result.data, "TIMEOUT");
                    recv_result.len = 0;
                    recv_result.error = ESP_FAIL;
                    subject_notify(recv_subject, &recv_result);
                    break;
                }
                if (recv_result.data[i] == ';') {
                    recv_result.data[i + 1] = '\0';
                    recv_result.len = i + 1;
                    recv_result.error = ESP_OK;
                    subject_notify(recv_subject, &recv_result);
                    break;
                }
                i++;
                if (i >= RECV_BUFFER_SIZE) {
                    ESP_LOGE(TAG, "Received data exceeds buffer size");
                    strcpy(recv_result.data, "OVERFLOW");
                    recv_result.len = 0;
                    recv_result.error = ESP_FAIL;
                    subject_notify(recv_subject, &recv_result);
                    break;
                }
            }
        }
    }
}

// Initialize the UART driver with interrupt-based reading
esp_err_t uart_init(void) {
    recv_subject = new_subject();

    send_queue = xQueueCreate(SEND_QUEUE_SIZE, sizeof(send_data_t));
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

void uart_add_recv_observer(observer_callback_t callback, void *context) {
    if (recv_subject == NULL) {
        ESP_LOGE(TAG, "Receive subject is not initialized");
        return;
    }
    add_observer(recv_subject, callback, context);
}

esp_err_t uart_send(const char *command, int type) {
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

    send_data_t send_data;
    send_data.type = type;
    strncpy(send_data.data, command, SEND_BUFFER_SIZE);
    send_data.len = command_size;

    // if (type == SEND_TYPE_MONITOR) {
    //     ESP_LOGI(TAG, "Sending command: %s", send_data.data);
    // }

    if (send_data.type == SEND_TYPE_EXTERNAL) {
        if (xQueueSendToFront(send_queue, &send_data, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Failed to enqueue command");
            return ESP_FAIL;
        }
    } else if (send_data.type == SEND_TYPE_MONITOR) {
        if (xQueueSend(send_queue, &send_data, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Failed to enqueue monitor");
            return ESP_FAIL;
        }
    } else {
        ESP_LOGE(TAG, "Command: %s, Invalid send type: %d", send_data.data, send_data.type);
        return ESP_FAIL;
    }

    return ESP_OK;
}
