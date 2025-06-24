#include "mem_chan.h"
#include <stdio.h>
#include <string.h>

#include "cJSON.h"

#include "esp_err.h"
#include "esp_log.h"

#include "command_list.h"
#include "rig_monitor.h"
#include "rig_controller.h"
#include "op_mode.h"
#include "ui.h"
#include "linked_list.h"

typedef enum {
    RT_REFRESH = 0,
    RT_FREQ_CHANGE,
    RT_SET_MEM_CHAN_BY_NAME,
    RT_SET_OP_MODE_BY_FREQUENCY,
    RT_SET_OP_MODE_BY_NAME,
    RT_DISABLE_FREQUENCY_UPDATE,
    RT_ENABLE_FREQUENCY_UPDATE,
    RT_NEXT_COMMAND,
    RT_TRANSMIT,
    RT_READY,
    RT_NOT_READY,
} rt_event_type_t;

typedef struct {
    rt_event_type_t type;
    char name[MAX_OP_MODE_NAME_LEN + 1];
    uint32_t frequency;
    bool transmit_enabled;
    bool op_mode_disabled;
} rt_event_t;

static QueueHandle_t rt_event_queue;
static bool transmit = false;
static uint32_t current_frequency = 999999999;
static linked_list_t *commands_list = NULL;
static char current_op_mode[MAX_OP_MODE_NAME_LEN + 1] = {0};
static char last_command[SEND_BUFFER_SIZE] = {0};
static char current_mem_chan[MAX_MEM_CHAN_NAME_LEN + 1] = {0};

#define TAG "RIG_CONTROLLER"

static void safe_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    } else {
        ESP_LOGE(TAG, "Attempted to free a NULL pointer");
    }
}

static void queue_frequency_change(uint32_t frequency, bool op_mode_disabled) {
    if (current_frequency == frequency) {
        // ESP_LOGI(TAG, "Frequency is already set to %lu", frequency);
        return;
    }
    // ESP_LOGI(TAG, "Frequency changed to: %lu", frequency);
            
    rt_event_t *rt_event = (rt_event_t *)calloc(1, sizeof(rt_event_t));
    rt_event->type = RT_FREQ_CHANGE;
    rt_event->frequency = frequency;
    rt_event->op_mode_disabled = op_mode_disabled;

    if (xQueueSend(rt_event_queue, &rt_event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue frequency change event");
    }
}

static void queue_event(rt_event_type_t event_type) {
    rt_event_t *rt_event = (rt_event_t *)calloc(1, sizeof(rt_event_t));
    rt_event->type = event_type;

    if (xQueueSend(rt_event_queue, &rt_event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue set op mode event");
    }
}

static void send_result(const char *result, const char *message) {
    if (result == NULL || message == NULL) {
        ESP_LOGE(TAG, "Invalid result or message");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (!json) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "controller");
    cJSON_AddStringToObject(json, "event", result);
    cJSON_AddStringToObject(json, "value", message);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str) {
        ESP_LOGI(TAG, "Result: %s", json_str);
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "Failed to print JSON");
    }

    cJSON_Delete(json);
}

static void error_log(const char *fmt, ...) {
    if (fmt == NULL) {
        ESP_LOGE(TAG, "Error message is NULL");
        return;
    }
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    ESP_LOGE(TAG, "%s", buf);
    send_result("error", buf);
}

static void handle_transmit(bool value) {
    rm_queue_tx_poll(value);
    transmit = value;
}

static void send_current_op_mode() {
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "controller");
    cJSON_AddStringToObject(json, "event", "current_op_mode");
    cJSON_AddStringToObject(json, "value", current_op_mode);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str != NULL) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "Failed to print JSON");
    }

    cJSON_Delete(json);
}

static void send_next_command() {
    if (linked_list_size(commands_list) == 0) {
        ESP_LOGI(TAG, "No commands left for current op mode");
        return;
    }

    char *cmd = linked_list_pop(commands_list);

    if (cmd == NULL) {
        ESP_LOGW(TAG, "No command found in current op mode");
        return;
    }

    if (strcmp(cmd, "RT_ENABLE_FREQUENCY_UPDATE") == 0) {
        queue_event(RT_ENABLE_FREQUENCY_UPDATE);
        safe_free(cmd);
        return;
    }

    if (!rc_needs_update(cmd)) {
        safe_free(cmd);
        send_next_command();
        return;
    }

    ESP_LOGI(TAG, "Send next command: %s", cmd);
    if (rm_queue_command(cmd, SEND_TYPE_COMMAND) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to queue command: %s", cmd);
        linked_list_push(commands_list, cmd);
        return;
    }
    strcpy(last_command, cmd);
    safe_free(cmd);
}

static void set_command_list(linked_list_t *list) {
    bool was_active = linked_list_size(commands_list) != 0;
    linked_list_destroy(commands_list, safe_free);

    commands_list = list;

    if (was_active) {
        ESP_LOGI(TAG, "sending next commands were already active");
    } else {
        send_next_command();
    }
}

static void set_current_op_mode(const char *name, linked_list_t *list) {
    if (name == NULL) {
        ESP_LOGE(TAG, "Cannot set current op mode to NULL");
        return;
    }

    if (strcmp(current_op_mode, name) == 0) {
        ESP_LOGI(TAG, "Current op mode is already set to: %s", current_op_mode);
        return;
    }

    ESP_LOGI(TAG, "Setting op mode to: %s", name);
    strcpy(current_op_mode, name);

    send_current_op_mode();

    if (linked_list_size(list) == 0) {
        ESP_LOGI(TAG, "No commands found for op mode: %s", name);
        return;
    }

    set_command_list(list);
}

static void send_current_mem_chan() {
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "controller");
    cJSON_AddStringToObject(json, "event", "current_mem_chan");
    cJSON_AddStringToObject(json, "value", current_mem_chan);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str != NULL) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "Failed to print JSON");
    }
    cJSON_Delete(json);
}

static void handle_set_mem_chan_by_name(const char *mc_name) {
    if (mc_name == NULL || strlen(mc_name) == 0) {
        error_log("Cannot set current mem_chan to NULL");
        return;
    }

    mem_chan_t *chan = mc_find_by_name(mc_name);
    if (chan == NULL) {
        ESP_LOGE(TAG, "No mem_chan found with name: %s", mc_name);
        return;
    }

    // No op_mode or commands specified, just queue the frequency change,
    // allow normal op_mode handling
    if (strlen(chan->op_mode) == 0 && strlen(chan->commands) == 0) {
        ESP_LOGI(TAG, "No op mode or commands, queueing frequency change: %lu", chan->frequency);

        queue_frequency_change(chan->frequency, false);
        mc_free_mem_chan(chan);
        return;
    }

    char *om_name = NULL;
    linked_list_t *om_list = NULL;
    if (strlen(chan->op_mode) > 0) { // Load the list of commands for the specified op_mode
        om_name = strdup(chan->op_mode);

        om_list = om_find_by_name(om_name);
        if (om_list == NULL) {
            ESP_LOGE(TAG, "Failed to find op mode by name: %s", om_name);

            safe_free(om_name);
            mc_free_mem_chan(chan);
            return;
        }
    } else {                        // Load the list of commands for the specified frequency
        om_list = om_find_by_frequency(&om_name, chan->frequency);
        if (om_name == NULL || om_list == NULL) {
            ESP_LOGE(TAG, "Failed to find op mode by frequency: %lu", chan->frequency);

            safe_free(om_name);
            mc_free_mem_chan(chan);
            return;
        }
    }

    // Load the list of commands for the specified mem_chan
    linked_list_t *mc_list = command_list_create(chan->commands);
    if (mc_list == NULL) {
        ESP_LOGE(TAG, "Failed to create commands list for mem_chan: %s", mc_name);

        linked_list_destroy(om_list, safe_free);
        safe_free(om_name);
        mc_free_mem_chan(chan);
        return;
    }

    // Merge the command lists, mc_list takes priority over om_list
    command_list_merge(om_list, mc_list);
    linked_list_destroy(mc_list, safe_free);

    queue_event(RT_DISABLE_FREQUENCY_UPDATE);

    // Sort the command list so mode, narrow and width are at the beginning.
    // Push a frequency command at the beginning of the list
    // and a marker command to enable frequency updates at the end

    command_list_sort(om_list);
    char *freq_cmd = rc_make_freq_command(chan->frequency);
    linked_list_shift(om_list, freq_cmd);
    current_frequency = chan->frequency;
    ESP_LOGI(TAG, "Setting current frequency to: %lu", current_frequency);

    char *enable_freq_update = strdup("RT_ENABLE_FREQUENCY_UPDATE");
    linked_list_push(om_list, enable_freq_update);

    set_current_op_mode(om_name, om_list);

    strncpy(current_mem_chan, mc_name, MAX_MEM_CHAN_NAME_LEN);
    send_current_mem_chan();

    safe_free(om_name);
    mc_free_mem_chan(chan);
}

static void handle_set_op_mode_by_frequency() {
    if (current_frequency == 0) {
        ESP_LOGE(TAG, "Invalid current_frequency value: 0");
        return;
    }

    char *name = NULL;
    linked_list_t *list = om_find_by_frequency(&name, current_frequency);
    if (name == NULL || list == NULL) {
        ESP_LOGE(TAG, "No op mode found for current_frequency: %lu", current_frequency);
        linked_list_destroy(list, safe_free);
        safe_free(name);
        return;
    }

    command_list_sort(list);
    set_current_op_mode(name, list);
    safe_free(name);
}

static void handle_set_op_mode_by_name(const char *name) {
    if (name == NULL || strlen(name) == 0) {
        error_log("Invalid op mode name");
        return;
    }

    linked_list_t *list = om_find_by_name(name);
    if (list == NULL) {
        error_log("Failed to find op mode by name: %s", name);
        return;
    }

    command_list_sort(list);
    set_current_op_mode(name, list);
}

static void handle_send_refresh() {
    send_current_op_mode();

    send_result("refresh", "complete");
}

static void handle_frequency_change(uint32_t frequency, bool op_mode_disabled) {
    if (frequency == 0) {
        ESP_LOGE(TAG, "Received frequency change with value 0");
        return;
    }

    if (current_frequency == frequency) {
        ESP_LOGI(TAG, "Frequency is already set to %lu", frequency);
        return;
    }

    current_frequency = frequency;
    ESP_LOGI(TAG, "Current frequency changed to: %lu", current_frequency);

    if (!op_mode_disabled) {
        queue_event(RT_SET_OP_MODE_BY_FREQUENCY);
    }
}

static void handle_next_command() {
    last_command[0] = '\0';
    send_next_command();
}

static void op_mode_task(void *pvParameters) {
    bool is_ready = false;
    bool disable_frequency_update = false;

    while(1) {
        rt_event_t *rt_event;

        if (xQueueReceive(rt_event_queue, &rt_event, portMAX_DELAY) == pdPASS) {
            switch(rt_event->type) {
            case RT_REFRESH:
                handle_send_refresh();
                break;
            case RT_FREQ_CHANGE:
                if (is_ready && !disable_frequency_update) {
                    handle_frequency_change(rt_event->frequency, rt_event->op_mode_disabled);
                }
                break;
            case RT_SET_MEM_CHAN_BY_NAME:
                handle_set_mem_chan_by_name(rt_event->name);
                break;
            case RT_SET_OP_MODE_BY_FREQUENCY:
                handle_set_op_mode_by_frequency();
                break;
            case RT_SET_OP_MODE_BY_NAME:
                handle_set_op_mode_by_name(rt_event->name);
                break;
            case RT_DISABLE_FREQUENCY_UPDATE:
                disable_frequency_update = true;
                break;
            case RT_ENABLE_FREQUENCY_UPDATE:
                disable_frequency_update = false;
                break;
            case RT_NEXT_COMMAND:
                handle_next_command();
                break;
            case RT_TRANSMIT:
                handle_transmit(rt_event->transmit_enabled);
                break;
            case RT_READY:
                is_ready = true;
                break;
            case RT_NOT_READY:
                is_ready = false;
                break;
            default:
                error_log("Unknown op mode event type: %d", rt_event->type);
                break;
            }
        }
        safe_free(rt_event);
    }
}

static void status_notification_handler(void *context, void *data) {
    ESP_LOGI(TAG, "Received status notification: %s", (char *)data);
    if (strcmp((char *)data, "ready") == 0) {
        queue_event(RT_READY);

        if (rm_queue_command(rc_freq_command(), SEND_TYPE_READ) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to queue get frequency command");
            return;
        }
    } else if (strcmp((char *)data, "not_ready") == 0) {
        queue_event(RT_NOT_READY);
    }
}

void rt_recv_from_ui(cJSON *json_obj) {
    cJSON *eventValue = cJSON_GetObjectItem(json_obj, "event");
    if (eventValue == NULL || !cJSON_IsString(eventValue)) {
        error_log("Missing or invalid event in JSON");
        return;
    }

    const char *event = eventValue->valuestring;
    ESP_LOGI(TAG, "Received event: %s", event);

    rt_event_t *rt_event = calloc(1, sizeof(rt_event_t));
    if (rt_event == NULL) {
        error_log("Failed to allocate memory for op mode event");
        return;
    }

    if (strcmp(event, "refresh") == 0) {
        rt_event->type = RT_REFRESH;
    } else if (strcmp(event, "set_op_mode") == 0) {
        cJSON *nameValue = cJSON_GetObjectItem(json_obj, "value");
        if (nameValue == NULL || !cJSON_IsString(nameValue)) {
            error_log("Missing or invalid value in JSON");
            safe_free(rt_event);
            return;
        }
        rt_event->type = RT_SET_OP_MODE_BY_NAME;
        strncpy(rt_event->name, nameValue->valuestring, MAX_OP_MODE_NAME_LEN);

    } else if (strcmp(event, "set_mem_chan") == 0) {
        cJSON *nameValue = cJSON_GetObjectItem(json_obj, "value");
        if (nameValue == NULL || !cJSON_IsString(nameValue)) {
            error_log("Missing or invalid value in JSON");
            safe_free(rt_event);
            return;
        }
        rt_event->type = RT_SET_MEM_CHAN_BY_NAME;
        strncpy(rt_event->name, nameValue->valuestring, MAX_MEM_CHAN_NAME_LEN);
    } else {
        error_log("Unknown event type: %s", event);
        safe_free(rt_event);
        return;
    }

    if (xQueueSend(rt_event_queue, &rt_event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        error_log("Failed to queue op event");
        safe_free(rt_event);
    }
}

static void command_notification_handler(void *context, void *data) {
    ESP_LOGI(TAG, "Received command: %s", (char *)data);
    if (rc_commands_are_same(last_command, data)) {
        queue_event(RT_NEXT_COMMAND);
    }

    uint32_t freq = rc_parse_frequency((const char *)data);
    if (freq != 0) {
        queue_frequency_change(freq, false);
    }
}

static void update_notification_handler(void *context, void *data) {
    uint32_t freq = rc_parse_frequency((const char *)data);
    if (freq != 0) {
        queue_frequency_change(freq, false);
    }
    
    char *command = (char *)data;
    if (memcmp(command, "TX", 2) == 0) {
        rt_event_t *rt_event = calloc(1, sizeof(rt_event_t));
        if (rt_event == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for transmit event");
            return;
        }

        rt_event->type = RT_TRANSMIT;
        rt_event->transmit_enabled = command[2] != '0';

        if (xQueueSend(rt_event_queue, &rt_event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to queue transmit event");
            safe_free(rt_event);
        }
        return;
    }
}

void rt_find_op_node_by_freq(void) {
    queue_event(RT_SET_OP_MODE_BY_FREQUENCY);
}

void init_rig_controller(void) {
    commands_list = linked_list_create();
    if (commands_list == NULL) {
        ESP_LOGE(TAG, "Failed to create linked list for op mode commands");
        return;
    }

    rt_event_queue = xQueueCreate(10, sizeof(rt_event_t *));
    if (rt_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create op mode event queue");
        return;
    }

    // create a task to process the op mode events
    if (xTaskCreate(op_mode_task, "op_mode_task", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create rig monitor task");
        return;
    }

    rig_monitor_add_observers(OBSERVE_STATUS, status_notification_handler, NULL);
    rig_monitor_add_observers(OBSERVE_COMMANDS, command_notification_handler, NULL);
    rig_monitor_add_observers(OBSERVE_UPDATES, update_notification_handler, NULL);
}
