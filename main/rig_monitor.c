#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "cat.h"
#include "info.h"
#include "observer.h"
#include "rig_commands.h"
#include "rig_monitor.h"

#define TAG "RIG_MONITOR"

typedef enum {
    RM_EVENT_RECEIVED = 0,
    RM_EVENT_TIMEOUT,
    RM_EVENT_SEND,
    RM_EVENT_SCAN,
    RM_EVENT_REFRESH,
    RM_EVENT_START,
} rm_event_type_t;

typedef struct {
    rm_event_type_t type;
    int priority;
    union {
        result_buf_t result_buf;
        command_buf_t command_buf;
    };
} rm_event_t;

static QueueHandle_t rm_event_queue;

static subject_t *command_subject = NULL;
static subject_t *updates_subject = NULL;
static subject_t *status_subject = NULL;

static TickType_t last_scan_tick;

/**
 * @brief Send the ID command to the radio.
 */
static void send_rig_id_command() {
    cat_send(rig_command_id(), SEND_TYPE_MONITOR, SEND_PRIORITY_HIGH);
}

void notify_observers(result_buf_t *result, bool updated) {
    if (result->command_buf.type == SEND_TYPE_COMMAND) {
        subject_notify(command_subject, (void *)result);
    }
    if (result->command_buf.type == SEND_TYPE_MONITOR && updated) {
        subject_notify(updates_subject, (void *)result);
    }
}

void notify_status(char *data) {
    ESP_LOGI(TAG, "Notify status: %s", data);
    result_buf_t result_buf;
    result_buf.result = RECV_RESULT_OK;
    strncpy(result_buf.data, data, RECV_BUFFER_SIZE);
    result_buf.len = strlen(data);
    subject_notify(status_subject, &result_buf);
}

// #define LOG_EVENTS
#ifdef LOG_EVENTS
static void log_event(rm_event_t *event, bool in_startup, bool is_ready) {
    char *type_str;
    switch(event->type) {
        case RM_EVENT_RECEIVED:
            type_str = "RECEIVED";
            break;
        case RM_EVENT_TIMEOUT:
            type_str = "TIMEOUT";
            break;
        case RM_EVENT_SEND:
            type_str = "SEND";
            break;
        case RM_EVENT_SCAN:
            type_str = "SCAN";
            break;
        case RM_EVENT_REFRESH:
            type_str = "REFRESH";
            break;
        case RM_EVENT_START:
            type_str = "START";
            break;
        default:
            type_str = "UNKNOWN";
            break;
    }
    ESP_LOGI(TAG, "Event: %s, in_startup: %s, is_ready: %s", type_str, 
             in_startup ? "true" : "false", is_ready ? "true" : "false");
}
#endif

/**
 * @brief Handle the results from sending commands to the radio.
 * 
 * @param in_startup Indicates if the monitor is in startup mode.
 * @param is_ready Indicates if the monitor is ready.
 * @param result Pointer to the received result.
 * 
 * @return see below.
 * 
 * When the monitor is in startup mode, it will only accept the ID command.
 * If the correct ID is received, the monitor will set in_startup to false, i.e. clear stupup mode.
 * If the ID command is not received, or the wrong id is received, return true and continue in startup mode.
 * Any other response will be silently ignored.
 * 
 * When the monitor is not in startup mode, it will accept any command and return the false.
 * Update the rig command last_value and next_refresh time.
 * If command is from command source, notify the command subject.
 * If command is from monitor source, notify the monitor subject if result if the command has changed.
 */
static bool event_received(bool in_startup, result_buf_t *result) {
    // log_result_buf("event_received", result);

    if (in_startup) {
        if (result->result == RECV_RESULT_OK) {
            if (strcmp(result->data, rig_expected_id()) == 0) {
                ESP_LOGI(TAG, "Received ID command: %s", result->data);
                ESP_LOGI(TAG, "Send queue length: %d", cat_send_len());
                return false;
            } else {
                ESP_LOGE(TAG, "Received wrong ID command: %s", result->data);
            }
        }

        send_rig_id_command();

        return true;
    }

    rig_command_t *cmd = find_command(result->command_buf.data);
    if (cmd == NULL) {
        ESP_LOGE(TAG, "Command not found: %s", result->command_buf.data);
        return false;
    }

    bool updated = rig_command_set_last_value(cmd, result->data);

    notify_observers(result, updated);

    return false;
}

/**
 * @brief Rig monitor task to handle events and send commands.
 * 
 * @param pvParameters Pointer to task parameters (not used).
 * 
 * There are three states for the rig monitor task:
 * 1. in_startup = true - 
 *    The monitor is in startup mode and will only send the ID command to the radio.
 *    The monitor will wait until the correct ID is received.
 *    Once the ID is received, the monitor will set in_startup to false.
 *    The rig commands will not be scaned for updates.
 *    Any other command will receive the error response "?;".
 * 
 * 2. in_startup = false && is_ready = false -
 *    Scan rig commands and send them to the radio.
 *    Once all the commands have been sent and responses received, is_ready will be set to true.
 *    Any commands sent by an command source will receive the error response "?;".
 * 
 * 3. in_startup = false && is_ready = true -
 *    Scan rig commands for commands that need to be refreshed and send them to the radio.
 *    Any commands sent by an command source will be sent to the radio.
 * 
 * If any timeout occurs, the monitor will reset and start over.
 */
static void rig_monitor_task(void *pvParameters) {
    bool in_startup = true;
    bool is_ready = false;

    while(1) {
        rm_event_t event;

        if (xQueueReceive(rm_event_queue, &event, portMAX_DELAY) == pdPASS) {
#ifdef LOG_EVENTS            
            log_event(&event, in_startup, is_ready);
#endif

            switch (event.type) { 
                case RM_EVENT_RECEIVED:
                    bool r = event_received(in_startup, &event.result_buf);
                    if (in_startup && !r) {
                        ESP_LOGI(TAG, "Rig monitor has finished startup");
                        in_startup = r;
                    }
                    break;

                case RM_EVENT_TIMEOUT:
                case RM_EVENT_START:
                    in_startup = true;
                    is_ready = false;
                    rig_command_reset();
                    cat_flush();
                    send_rig_id_command();
                    ESP_LOGW(TAG, "Rig monitor timeout, resetting");
                    notify_status(ENHANCED_RIG_RESULT_NOT_READY);
                    break;

                case RM_EVENT_SEND:
                    if (in_startup || !is_ready) {
                        notify_status(ENHANCED_RIG_RESULT_NOT_READY);
                    } else {
                        if (cat_send(event.command_buf.data, event.command_buf.type, event.priority) == ESP_ERR_NO_MEM) {
                            notify_status(ENHANCED_RIG_RESULT_BUSY);
                            get_info()->send_queue_full++;
                        }
                    }
                    break;

                case RM_EVENT_SCAN:
                    if (!in_startup) {
                        if (!is_ready) {
                            if (rig_command_is_ready()) {
                                is_ready = true;
                                notify_status(ENHANCED_RIG_RESULT_READY);
                                ESP_LOGI(TAG, "Rig monitor is ready");
                            }
                        } else {
                            last_scan_tick = rig_command_scan_for_updates(last_scan_tick);
                        }
                    }
                    break;

                case RM_EVENT_REFRESH:
                    rig_command_refresh();
                    break;

                default:
                    ESP_LOGE(TAG, "Unknown event type: %d", event.type);
                    break;
            }
        }
    }
}

void rig_monitor_add_observers(uint32_t observed, observer_callback_t callback, void *context) {
    if (updates_subject == NULL) {
        ESP_LOGE(TAG, "Receive subject is not initialized");
        return;
    }
    if (observed & OBSERVE_UPDATES) {
        add_observer(updates_subject, callback, context);
    }
    if (observed & OBSERVE_COMMANDS) {
        add_observer(command_subject, callback, context);
    }
    if (observed & OBSERVE_STATUS) {
        add_observer(status_subject, callback, context);
    }
}

void rig_monitor_remove_observers(observer_callback_t callback) {
    if (updates_subject == NULL) {
        ESP_LOGE(TAG, "Receive subject is not initialized");
        return;
    }
    remove_observer(updates_subject, callback);
    remove_observer(command_subject, callback);
    remove_observer(status_subject, callback);
}

esp_err_t rig_monitor_send(const char *command, int type) {
    size_t command_size = strnlen(command, SEND_BUFFER_SIZE);
    if (command_size == 0) {
        ESP_LOGE(TAG, "Command is empty");
        return ESP_FAIL;
    }
    // Buffer needs to be large enough to hold the command and the null terminator
    if (command_size >= SEND_BUFFER_SIZE) {
        ESP_LOGE(TAG, "Command size %d exceeds buffer size %d", command_size, SEND_BUFFER_SIZE - 1);
        return ESP_FAIL;
    }
    if (command[command_size - 1] != ';') {
        ESP_LOGE(TAG, "Command must end with a semicolon");
        return ESP_FAIL;
    }

    rm_event_t event;

    if (strcmp(command, ENHANCED_RIG_COMMAND_REFRESH) == 0) {
        event.type = RM_EVENT_REFRESH;
    } else {
        event.type = RM_EVENT_SEND;
        event.priority = type == SEND_TYPE_COMMAND ? SEND_PRIORITY_HIGH : SEND_PRIORITY_NORMAL;
        event.command_buf.type = type;
        strncpy(event.command_buf.data, command, SEND_BUFFER_SIZE);
        event.command_buf.len = command_size;
    }

    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send event to rig monitor task");
    }

    return ESP_OK;
}

void rig_monitor_recv_data(result_buf_t *result) {
    rm_event_t event;

    switch(result->result) {
    case RECV_RESULT_OK:
        // log_result_buf("rig_monitor_recv_data", result);
        event.type = RM_EVENT_RECEIVED;
        memcpy(&event.result_buf, result, sizeof(result_buf_t));
        while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send event to rig monitor task");
        }
        break;
    case RECV_RESULT_TIMEOUT:
        event.type = RM_EVENT_TIMEOUT;
        while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send event to rig monitor task");
        }
        break;
    case RECV_RESULT_OVERFLOW:
        ESP_LOGE(TAG, "Received data overflow");
        break;
    case RECV_RESULT_ERROR:
        ESP_LOGE(TAG, "Error receiving data: %s", result->data);
        break;
    }
}

void rig_monitor_command_buf(int type, const char *data) {
    command_buf_t command_buf;
    command_buf.type = type;
    strncpy(command_buf.data, data, SEND_BUFFER_SIZE);
    command_buf.len = strlen(data);
    if (command_buf.len > SEND_BUFFER_SIZE) {
        ESP_LOGE(TAG, "Send data length exceeds buffer size");
        return;
    }

    rm_event_t event;
    event.type = RM_EVENT_SEND;
    memcpy(&event.result_buf.command_buf, &command_buf, sizeof(command_buf_t));
    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send event to rig monitor task");
    }
}

static void rig_monitor_scan_timer(TimerHandle_t xTimer) {
    rm_event_t event;
    event.type = RM_EVENT_SCAN;
    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send scan event to rig monitor task");
    }
}

void init_rig_monitor() {
    last_scan_tick = xTaskGetTickCount();

    command_subject = new_subject();
    updates_subject = new_subject();
    status_subject = new_subject();

    rm_event_queue = xQueueCreate(10, sizeof(rm_event_t));
    if (rm_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create rig monitor event queue");
        return;
    }

    // create a task to process the rig monitor events
    if (xTaskCreate(rig_monitor_task, "rig_monitor_task", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create rig monitor task");
        return;
    }

    // create a timer to periodically scan the rig commands for updates

    TimerHandle_t timer = xTimerCreate("rig_monitor_scan_timer", pdMS_TO_TICKS(POLL_INTERVAL), pdTRUE, NULL, rig_monitor_scan_timer);
    if (xTimerStart(timer, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to start rig monitor timer");
        return;
    }

    rm_event_t event;
    event.type = RM_EVENT_START;
    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send event to rig monitor task");
    }

    ESP_LOGI(TAG, "Rig monitor initialized");
}
