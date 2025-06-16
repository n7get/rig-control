#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "config.h"
#include "http.h"
#include "settings.h"
#include "ui.h"

static const char *TAG = "SETTINGS";

char ap_ssid[32] = "rig-controller";
char ap_password[64] = "";
char sta_ssid[32] = "";
char sta_password[64] = "";

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

    cJSON_AddStringToObject(json, "topic", "settings");
    cJSON_AddStringToObject(json, "event", result);
    cJSON_AddStringToObject(json, "value", message);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str) {
        ESP_LOGI(TAG, "Result: %s", json_str);
        ui_send_json(json_str);
        free(json_str);
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

void load_settings(void) {
    if (get_string("ap_ssid", ap_ssid, sizeof(ap_ssid)) == ESP_OK) {
        ESP_LOGI(TAG, "Loaded AP SSID from NVS: %s", ap_ssid);
    } else {
        ESP_LOGW(TAG, "Failed to load AP SSID from NVS, using default: %s", ap_ssid);
        set_string("ap_ssid", ap_ssid);
        ESP_LOGI(TAG, "Default AP SSID saved to NVS: %s", ap_ssid);
    }

    if (get_string("ap_password", ap_password, sizeof(ap_password)) == ESP_OK) {
        ESP_LOGI(TAG, "Loaded AP Password from NVS: %s", ap_password);
    } else {
        ESP_LOGW(TAG, "Failed to load AP Password from NVS, using default: %s", ap_password);
        set_string("ap_password", ap_password);
        ESP_LOGI(TAG, "Default AP Password saved to NVS: %s", ap_password);
    }

    if (get_string("sta_ssid", sta_ssid, sizeof(sta_ssid)) == ESP_OK) {
        ESP_LOGI(TAG, "Loaded STA SSID from NVS: %s", sta_ssid);
    } else {
        ESP_LOGW(TAG, "Failed to load STA SSID from NVS, using default: %s", sta_ssid);
        set_string("sta_ssid", sta_ssid);
        ESP_LOGI(TAG, "Default STA SSID saved to NVS: %s", sta_ssid);
    }

    if (get_string("sta_password", sta_password, sizeof(sta_password)) == ESP_OK) {
        ESP_LOGI(TAG, "Loaded STA Password from NVS: %s", sta_password);
    } else {
        ESP_LOGW(TAG, "Failed to load STA Password from NVS, using default: %s", sta_password);
        set_string("sta_password", sta_password);
        ESP_LOGI(TAG, "Default STA Password saved to NVS: %s", sta_password);
    }
}

static void handle_set(cJSON *json) {
    if (!json) {
        error_log("Invalid JSON object");
        return;
    }

    cJSON *value = cJSON_GetObjectItem(json, "value");
    if (!value || !cJSON_IsObject(value)) {
        error_log("Invalid or missing 'value' object in JSON");
        cJSON_Delete(json);
        return;
    }
    
    cJSON *ap_ssid_json = cJSON_GetObjectItem(value, "ap_ssid");
    if (ap_ssid_json == NULL || !cJSON_IsString(ap_ssid_json)) {
        error_log("AP SSID parameter missing or invalid");
        cJSON_Delete(json);
        return;
    }

    cJSON *ap_password_json = cJSON_GetObjectItem(value, "ap_password");
    if (ap_password_json == NULL || !cJSON_IsString(ap_password_json)) {
        error_log("AP Password parameter missing or invalid");
        cJSON_Delete(json);
        return;
    }

    cJSON *sta_ssid_json = cJSON_GetObjectItem(value, "sta_ssid");
    if (sta_ssid_json == NULL || !cJSON_IsString(sta_ssid_json)) {
        error_log("STA SSID parameter missing or invalid");
        cJSON_Delete(json);
        return;
    }

    cJSON *sta_password_json = cJSON_GetObjectItem(value, "sta_password");
    if (sta_password_json == NULL || !cJSON_IsString(sta_password_json)) {
        error_log("STA Password parameter missing or invalid");
        cJSON_Delete(json);
        return;
    }

    if (strcmp(ap_ssid, ap_ssid_json->valuestring) != 0) {
        strncpy(ap_ssid, ap_ssid_json->valuestring, sizeof(ap_ssid) - 1);
        ap_ssid[sizeof(ap_ssid) - 1] = '\0';
        set_string("ap_ssid", ap_ssid);
        ESP_LOGI(TAG, "AP SSID updated and saved to NVS: %s", ap_ssid);
    }

    if (strcmp(ap_password, ap_password_json->valuestring) != 0) {
        strncpy(ap_password, ap_password_json->valuestring, sizeof(ap_password) - 1);
        ap_password[sizeof(ap_password) - 1] = '\0';
        set_string("ap_password", ap_password);
        ESP_LOGI(TAG, "AP Password updated and saved to NVS: %s", ap_password);
    }

    if (strcmp(sta_ssid, sta_ssid_json->valuestring) != 0) {
        strncpy(sta_ssid, sta_ssid_json->valuestring, sizeof(sta_ssid) - 1);
        sta_ssid[sizeof(sta_ssid) - 1] = '\0';
        set_string("sta_ssid", sta_ssid);
        ESP_LOGI(TAG, "STA SSID updated and saved to NVS: %s", sta_ssid);
    }

    if (strcmp(sta_password, sta_password_json->valuestring) != 0) {
        strncpy(sta_password, sta_password_json->valuestring, sizeof(sta_password) - 1);
        sta_password[sizeof(sta_password) - 1] = '\0';
        set_string("sta_password", sta_password);
        ESP_LOGI(TAG, "STA Password updated and saved to NVS: %s", sta_password);
    }

    cJSON_Delete(json);
    
    send_result("response", "ok");
}

static void handle_get(void) {
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        error_log("Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "settings");
    cJSON_AddStringToObject(json, "event", "response");

    cJSON *value = cJSON_CreateObject();
    if (!value) {
        error_log("Failed to create JSON object");
        cJSON_Delete(json);
        return;
    }

    cJSON_AddStringToObject(value, "ap_ssid", ap_ssid);
    cJSON_AddStringToObject(value, "ap_password", ap_password);
    cJSON_AddStringToObject(value, "sta_ssid", sta_ssid);
    cJSON_AddStringToObject(value, "sta_password", sta_password);

    cJSON_AddItemToObject(json, "value", value);

    const char *response = cJSON_Print(json);
    if (!response) {
        error_log("Failed to print JSON object");
        cJSON_Delete(json);
        return;
    }

    ui_send_json(response);

    cJSON_Delete(json);
    free((void *)response);
}

void st_recv_from_ui(cJSON *json) {
    cJSON *eventValue = cJSON_GetObjectItem(json, "event");
    if (eventValue == NULL || !cJSON_IsString(eventValue)) {
        error_log("No event found in JSON or event is not a string");
        return;
    }
    const char *event = eventValue->valuestring;

    if (strcmp(event, "get") == 0) {
        handle_get();
    } else if (strcmp(event, "set") == 0) {
        handle_set(json);
    } else {
        error_log("Received unknown event: %s", event);
    }
}
