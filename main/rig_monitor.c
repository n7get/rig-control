#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_timer.h"
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
    RM_EVENT_PING,
    RM_TX_POLL_SET,
    RM_TX_POLL_CLEAR,
} rm_event_type_t;

typedef struct {
    rm_event_type_t type;
    int priority;
    union {
        command_t command;
        response_t response;
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
    command_t command;
    memset(&command, 0, sizeof(command_t));
    command.type = SEND_TYPE_READ;
    strncpy(command.read, rc_id_command(), SEND_BUFFER_SIZE);
    command.read_len = strlen(command.read);

    cat_queue_command(&command, SEND_PRIORITY_HIGH);
}

void notify_observers(response_t *response, bool updated) {
    if (response->type == SEND_TYPE_COMMAND) {
        subject_notify(command_subject, (void *)response->response);
    }
    if (response->type == SEND_TYPE_READ && updated) {
        subject_notify(updates_subject, (void *)response->response);
    }
}

static void notify_refresh(char *data) {
    // ESP_LOGI(TAG, "Sending refresh notification: %s", data);
    subject_notify(updates_subject, data);
}

void notify_status(const char *data) {
    subject_notify(status_subject, (void *)data);
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
        case RM_EVENT_PING:
            type_str = "PING";
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
static bool event_received(bool in_startup, response_t *response) {
    // log_response("event_received", response);

    if (in_startup) {
        if (response->result == RECV_RESULT_OK) {
            if (strcmp(response->response, rc_is_id()) == 0) {
                ESP_LOGI(TAG, "Received ID command: %s", response->response);
                ESP_LOGI(TAG, "Send queue length: %d", cat_send_len());
                return false;
            } else {
                ESP_LOGE(TAG, "Received wrong ID command: %s", response->response);
            }
        }

        send_rig_id_command();

        return true;
    }

    bool updated = rc_set_last_value(response);

    notify_observers(response, updated);

    return false;
}

static void event_ping(bool in_startup, bool is_ready) {
    notify_status(rc_result_ping());
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
    info_t *info = get_info();

    while(1) {
        rm_event_t event;

        info->rm_queue_count = uxQueueMessagesWaiting(rm_event_queue);
        if (xQueueReceive(rm_event_queue, &event, portMAX_DELAY) == pdPASS) {
            info->rm_queue_polls++;
            info->last_rm_queue_event = event.type;
#ifdef LOG_EVENTS            
            log_event(&event, in_startup, is_ready);
#endif

            switch (event.type) { 
                case RM_EVENT_RECEIVED:
                    bool r = event_received(in_startup, &event.response);
                    if (in_startup && !r) {
                        ESP_LOGI(TAG, "Rig monitor has finished startup");
                        in_startup = r;
                    }
                    break;

                case RM_EVENT_TIMEOUT:
                case RM_EVENT_START:
                    in_startup = true;
                    is_ready = false;
                    rc_reset();
                    cat_flush();
                    send_rig_id_command();
                    ESP_LOGW(TAG, "Restarting rig monitor");
                    notify_status(rc_result_not_ready());
                    break;

                case RM_EVENT_SEND:
                    if (in_startup || !is_ready) {
                        notify_status(rc_result_not_ready());
                    } else {
                        if (cat_queue_command(&event.command, event.priority) == ESP_ERR_NO_MEM) {
                            notify_status(rc_result_busy());
                            get_info()->cat_queue_full++;
                        }
                    }
                    break;

                case RM_EVENT_SCAN:
                    if (in_startup) {
                        break;
                    }

                    if (!is_ready) {
                        if (!rc_is_ready()) {
                            break;
                        }

                        rc_randomize_refresh();
                        is_ready = true;
                        notify_status(rc_result_ready());
                        ESP_LOGI(TAG, "Rig monitor is ready");
                    }

                    last_scan_tick = rc_scan_for_updates(last_scan_tick);
                    break;

                case RM_EVENT_REFRESH:
                    ESP_LOGI(TAG, "Starting refresh");
                    rc_send_refresh(notify_refresh);
                    notify_status(rc_result_ready());
                    ESP_LOGI(TAG, "Refresh complete");
                    break;

                case RM_EVENT_PING:
                    event_ping(in_startup, is_ready);
                    break;

                case RM_TX_POLL_SET:
                    rc_set_tx_poll();
                    break;
                case RM_TX_POLL_CLEAR:
                    rc_clear_tx_poll();
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

static esp_err_t setup_event(rm_event_t *event, const char *cmd_str, send_type_t type) {
    event->type = RM_EVENT_SEND;
    event->priority = type == SEND_PRIORITY_HIGH;
    esp_err_t err = setup_command(&event->command, cmd_str, type);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to setup command: %s", cmd_str);
        return err;
    }

    return ESP_OK;
}


esp_err_t rm_queue_command(const char *cmd_str, send_type_t type) {
    rm_event_t event;
    memset(&event, 0, sizeof(rm_event_t));

    esp_err_t err = setup_event(&event, cmd_str, type);
    if (err != ESP_OK) {
        return err;
    }

    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send event to rig monitor task");
    }

    return ESP_OK;
}

void rm_queue_refresh() {
    rm_event_t event;
    event.type = RM_EVENT_REFRESH;
    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send refresh event to rig monitor task");
    }
}
void rm_queue_ping() {
    rm_event_t event;
    event.type = RM_EVENT_PING;
    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send ping event to rig monitor task");
    }
}

void rm_queue_tx_poll(bool tx_poll) {
    rm_event_t event;
    if (tx_poll) {
        event.type = RM_TX_POLL_SET;
    } else {
        event.type = RM_TX_POLL_CLEAR;
    }

    while (xQueueSend(rm_event_queue, &event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send tx poll event to rig monitor task");
    }
}

void rm_queue_response(response_t *response) {
    rm_event_t event;

    switch(response->result) {
    case RECV_RESULT_OK:
        // log_response("rig_monitor_recv_data", response);
        event.type = RM_EVENT_RECEIVED;
        memcpy(&event.response, response, sizeof(response_t));
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
        ESP_LOGE(TAG, "Error receiving data: %s", response->response);
        break;
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
