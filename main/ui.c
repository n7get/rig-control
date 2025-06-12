#include <string.h>

#include "cJSON.h"

#include "esp_log.h"

#include "mem_chan.h"
#include "observer.h"
#include "op_mode.h"
#include "rig_commands.h"
#include "rig_controller.h"
#include "rig_monitor.h"
#include "ui.h"
#include "web_socket.h"

#define TAG "UI"

static void status_message_handler(void *context, void *data) {
    if (data == NULL) {
        ESP_LOGE(TAG, "Received null data in status_message_handler");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "monitor");
    cJSON_AddStringToObject(json, "event", "status");
    cJSON_AddStringToObject(json, "value", data);

    char *json_string = cJSON_PrintUnformatted(json);
    if (json_string == NULL) {
        ESP_LOGE(TAG, "Failed to print JSON object");
        cJSON_Delete(json);
        return;
    }

    // ESP_LOGI(TAG, "Sending controller notification: %s", json_string);
    ws_send(json_string);

    cJSON_free(json_string);
    cJSON_Delete(json);
}

static void update_message_handler(void *context, void *data) {
    if (data == NULL) {
        ESP_LOGE(TAG, "Received null response in update handler");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "monitor");
    cJSON_AddStringToObject(json, "event", "update");
    cJSON_AddStringToObject(json, "value", data);

    char *json_string = cJSON_PrintUnformatted(json);
    if (json_string == NULL) {
        ESP_LOGE(TAG, "Failed to print JSON object");
        cJSON_Delete(json);
        return;
    }

    // ESP_LOGI(TAG, "Sending notification: %s", json_string);
    ws_send(json_string);

    cJSON_free(json_string);
    cJSON_Delete(json);
}

void ui_send_json(const char *json) {
    ws_send(json);
}

void ui_handle_json(cJSON *json_obj) {
    cJSON *topicValue = cJSON_GetObjectItem(json_obj, "topic");
    if (topicValue == NULL) {
        ESP_LOGE(TAG, "No topic found in JSON");
        return;
    }
    char *topic = topicValue->valuestring;

    if (strcmp(topic, "monitor") == 0) {
        cJSON *eventValue = cJSON_GetObjectItem(json_obj, "event");
        if (eventValue == NULL) {
            ESP_LOGE(TAG, "No monitor event found in JSON");
            return;
        }
        char *event = eventValue->valuestring;
        ESP_LOGI(TAG, "Received monitor event: %s", event);

        if (strcmp(event, "refresh") == 0) {
            rm_queue_refresh();
            return;
        }

        cJSON *valueValue = cJSON_GetObjectItem(json_obj, "value");
        if (valueValue == NULL || !cJSON_IsString(valueValue)) {
            ESP_LOGE(TAG, "Missing or invalid value in JSON");
            return;
        }
        char *value = valueValue->valuestring;

        if (strcmp(event, "command") == 0) {
            rm_queue_command(value, SEND_TYPE_COMMAND);
            return;
        }

        if (strcmp(event, "read") == 0) {
            rm_queue_command(value, SEND_TYPE_READ);
            return;
        }

        ESP_LOGW(TAG, "Unknown monitor event: %s", event);
        return;
    }
    
    if (strcmp(topic, "controller") == 0) {
        rt_recv_from_ui(json_obj);
        return;
    }

    if (strcmp(topic, "mem_chan") == 0) {
        mc_recv_from_ui(json_obj);
        return;
    }

    if (strcmp(topic, "op_mode") == 0) {
        om_recv_from_ui(json_obj);
        return;
    }

    ESP_LOGE(TAG, "Unknown topic: %s", topic);
}

void ui_recv_json(const char *json) {
    if (json == NULL) {
        ESP_LOGE(TAG, "Received null json");
        return;
    }

    cJSON *json_obj = cJSON_Parse(json);
    if (json_obj == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return;
    }

    ui_handle_json(json_obj);

    cJSON_Delete(json_obj);
}

void ui_init() {
    rig_monitor_add_observers(OBSERVE_COMMANDS | OBSERVE_UPDATES, update_message_handler, NULL);
    rig_monitor_add_observers(OBSERVE_STATUS, status_message_handler, NULL);

    ESP_LOGI(TAG, "UI module initialized and registered for notifications");
}
