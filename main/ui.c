#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "observer.h"
#include "op_mode.h"
#include "rig_commands.h"
#include "rig_monitor.h"
#include "ui.h"

#define TAG "UI"

subject_t *ui_subject = NULL;

void ui_add_observers(observer_callback_t callback, void *context) {
    if (ui_subject == NULL) {
        ESP_LOGE(TAG, "UI subject is not initialized");
        return;
    }

    add_observer(ui_subject, callback, context);
}

void ui_remove_observers(observer_callback_t callback) {
    if (ui_subject == NULL) {
        ESP_LOGE(TAG, "UI subject is not initialized");
        return;
    }

    remove_observer(ui_subject, callback);
}

static void notify(const char *message) {
    if (ui_subject == NULL) {
        ESP_LOGE(TAG, "UI subject is not initialized");
        return;
    }

    subject_notify(ui_subject, (void *)message);
}

static void notify_control(void *context, void *data) {
    if (data == NULL) {
        ESP_LOGE(TAG, "Received null data in notify_control");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "control");
    cJSON_AddStringToObject(json, "event", data);

    char *json_string = cJSON_PrintUnformatted(json);
    if (json_string == NULL) {
        ESP_LOGE(TAG, "Failed to print JSON object");
        cJSON_Delete(json);
        return;
    }

    // ESP_LOGI(TAG, "Sending control notification: %s", json_string);
    notify(json_string);

    cJSON_free(json_string);
    cJSON_Delete(json);
}

static void ui_notification_handler(void *context, void *data) {
    if (data == NULL) {
        ESP_LOGE(TAG, "Received null response in notification handler");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "update");
    cJSON_AddStringToObject(json, "value", data);

    char *json_string = cJSON_PrintUnformatted(json);
    if (json_string == NULL) {
        ESP_LOGE(TAG, "Failed to print JSON object");
        cJSON_Delete(json);
        return;
    }

    // ESP_LOGI(TAG, "Sending notification: %s", json_string);
    notify(json_string);

    cJSON_free(json_string);
    cJSON_Delete(json);
}

void ui_send_json(const char *json) {
    notify(json);
}

void ui_handle_json(cJSON *json_obj) {
    cJSON *topicValue = cJSON_GetObjectItem(json_obj, "topic");
    if (topicValue == NULL) {
        ESP_LOGE(TAG, "No topic found in JSON");
        return;
    }
    char *topic = topicValue->valuestring;

    if (strcmp(topic, "command") == 0) {
        cJSON *cmdValue = cJSON_GetObjectItem(json_obj, "command");
        if (cmdValue == NULL) {
            ESP_LOGE(TAG, "No command found in JSON");
            return;
        }
        char *command = cmdValue->valuestring;
        ESP_LOGI(TAG, "Received command: %s", command);

        rm_queue_command(command, SEND_TYPE_COMMAND);
        return;
    }

    if (strcmp(topic, "read") == 0) {
        cJSON *cmdValue = cJSON_GetObjectItem(json_obj, "read");
        if (cmdValue == NULL) {
            ESP_LOGE(TAG, "No read found in JSON");
            return;
        }
        char *read = cmdValue->valuestring;
        ESP_LOGI(TAG, "Received read: %s", read);

        rm_queue_command(read, SEND_TYPE_READ);
        return;
    }
    
    if (strcmp(topic, "control") == 0) {
        cJSON *eventValue = cJSON_GetObjectItem(json_obj, "event");
        if (eventValue == NULL) {
            ESP_LOGE(TAG, "No event found in JSON");
            return;
        }
        char *event = eventValue->valuestring;
        ESP_LOGI(TAG, "Received event: %s", event);
        
        if (strcmp(event, "refresh") == 0) {
            rm_queue_refresh();
            return;
        }
        
        if (strcmp(event, "ping") == 0) {
            rm_queue_ping();
            return;
        }

        ESP_LOGE(TAG, "Control event not implemented: %s", event);
        return;
    }

    if (strcmp(topic, "op_mode") == 0) {
        om_recv_from_ui(json_obj);
        return;
    }

    ESP_LOGE(TAG, "Unknown topic: %s", topic);
}

void ui_recv(const char *json) {
    if (json == NULL) {
        ESP_LOGE(TAG, "Received null json");
        return;
    }

    ESP_LOGI(TAG, "Received json: %s", json);
    cJSON *json_obj = cJSON_Parse(json);
    if (json_obj == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return;
    }

    ui_handle_json(json_obj);

    cJSON_Delete(json_obj);
}

void ui_init() {
    ui_subject = new_subject();
    if (ui_subject == NULL) {
        ESP_LOGE(TAG, "Failed to create UI subject");
        return;
    }

    rig_monitor_add_observers(OBSERVE_COMMANDS | OBSERVE_UPDATES, ui_notification_handler, NULL);
    rig_monitor_add_observers(OBSERVE_STATUS, notify_control, NULL);

    ESP_LOGI(TAG, "UI module initialized and registered for notifications");
}
