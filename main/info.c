#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "config.h"
#include "http.h"
#include "info.h"
#include "ui.h"

#define TAG "INFO"

info_t info;

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

    cJSON_AddStringToObject(json, "topic", "info");
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

info_t *get_info() {
    return &info;
}

void init_info() {
    info.cat_empty_queue = 0;
    info.cat_queue_busy = 0;
    info.cat_max_queue_size = 0;
    info.cat_max_send_len = 0;
    info.cat_max_receive_len = 0;
    info.cat_total_response_time = 0;
    info.cat_no_responses = 0;
    info.cat_max_response_time = 0;
    info.rc_polls = 0;
    info.rc_elapsed_ticks = 0;
    info.rc_pending = 0;
    info.rc_valid = 0;
    info.rc_updates = 0;
    info.rt_empty_queue = 0;
    info.rt_queue_busy = 0;
    info.rt_max_queue_size = 0;
    info.rm_empty_queue = 0;
    info.rm_queue_busy = 0;
    info.rm_max_queue_size = 0;
    info.rm_last_queue_event = -1;
    info.cat_queue_full = 0;
    info.cat_queue_fast_full = 0;
    info.uart_empty_queue = 0;
    info.uart_queue_busy = 0;
    info.uart_max_queue_size = 0;
    info.uart_max_read_len = 0;
    info.uart_buffer_full = 0;
    info.uart_fifo_ovf = 0;
    info.uart_input_queue_full = 0;
    info.ws_max_queue_size = 0;
    info.ws_empty_queue = 0;
    info.ws_queue_busy = 0;
}

static void handle_get() {
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        error_log("Failed to create JSON object");
        return;
    }
    cJSON_AddStringToObject(json, "topic", "info");
    cJSON_AddStringToObject(json, "event", "response");

    cJSON *value = cJSON_CreateObject();
    if (value == NULL) {
        error_log("Failed to create JSON value object");
        cJSON_Delete(json);
        return;
    }

    cJSON_AddNumberToObject(value, "cat_empty_queue", info.cat_empty_queue);
    cJSON_AddNumberToObject(value, "cat_queue_busy", info.cat_queue_busy);
    cJSON_AddNumberToObject(value, "cat_max_queue_size", info.cat_max_queue_size);
    
    cJSON_AddNumberToObject(value, "cat_max_send_len", info.cat_max_send_len);
    cJSON_AddNumberToObject(value, "cat_max_receive_len", info.cat_max_receive_len);
    float cat_avg_response_time = (float)info.cat_total_response_time / (float)info.cat_no_responses;
    cJSON_AddNumberToObject(value, "cat_avg_response_time", cat_avg_response_time / 1000.0f);
    cJSON_AddNumberToObject(value, "cat_max_response_time", info.cat_max_response_time / 1000);

    float rc_avg_elapsed_ticks = (float)info.rc_elapsed_ticks / (float)info.rc_polls;
    cJSON_AddNumberToObject(value, "rc_avg_elapsed_ticks", rc_avg_elapsed_ticks);
    float rc_avg_pending = (float)info.rc_pending / (float)info.rc_polls;
    cJSON_AddNumberToObject(value, "rc_avg_pending", rc_avg_pending);
    float rc_avg_valid = (float)info.rc_valid / (float)info.rc_polls;
    cJSON_AddNumberToObject(value, "rc_avg_valid", rc_avg_valid);
    float rc_avg_updates = (float)info.rc_updates / (float)info.rc_polls;
    cJSON_AddNumberToObject(value, "rc_avg_updates", rc_avg_updates);

    cJSON_AddNumberToObject(value, "rt_empty_queue", info.rt_empty_queue);
    cJSON_AddNumberToObject(value, "rt_queue_busy", info.rt_queue_busy);
    cJSON_AddNumberToObject(value, "rt_max_queue_size", info.rt_max_queue_size);

    cJSON_AddNumberToObject(value, "rm_empty_queue", info.rm_empty_queue);
    cJSON_AddNumberToObject(value, "rm_queue_busy", info.rm_queue_busy);
    cJSON_AddNumberToObject(value, "rm_max_queue_size", info.rm_max_queue_size);
    cJSON_AddNumberToObject(value, "rm_last_queue_event", info.rm_last_queue_event);
    
    cJSON_AddNumberToObject(value, "cat_queue_full", info.cat_queue_full);
    cJSON_AddNumberToObject(value, "cat_queue_fast_full", info.cat_queue_fast_full);

    cJSON_AddNumberToObject(value, "uart_empty_queue", info.uart_empty_queue);
    cJSON_AddNumberToObject(value, "uart_queue_busy", info.uart_queue_busy);
    cJSON_AddNumberToObject(value, "uart_max_queue_size", info.uart_max_queue_size);
    cJSON_AddNumberToObject(value, "uart_max_read_len", info.uart_max_read_len);
    cJSON_AddNumberToObject(value, "uart_buffer_full", info.uart_buffer_full);
    cJSON_AddNumberToObject(value, "uart_fifo_ovf", info.uart_fifo_ovf);
    cJSON_AddNumberToObject(value, "uart_input_queue_full", info.uart_input_queue_full);

    cJSON_AddNumberToObject(value, "ws_max_queue_size", info.ws_max_queue_size);
    cJSON_AddNumberToObject(value, "ws_empty_queue", info.ws_empty_queue);
    cJSON_AddNumberToObject(value, "ws_queue_busy", info.ws_queue_busy);

    cJSON_AddNumberToObject(value, "free_heap", esp_get_free_heap_size());
    cJSON_AddNumberToObject(value, "min_free_heap", esp_get_minimum_free_heap_size());
    cJSON_AddNumberToObject(value, "free_stack", uxTaskGetStackHighWaterMark(NULL));

    nvs_stats_t nvs_stats;
    if (nvs_get_stats("nvs", &nvs_stats) == ESP_OK) {
        cJSON_AddNumberToObject(value, "nvs_used_bytes", nvs_stats.used_entries * 32);
        cJSON_AddNumberToObject(value, "nvs_free_bytes", nvs_stats.free_entries * 32);
        cJSON_AddNumberToObject(value, "nvs_available_bytes", nvs_stats.available_entries * 32);
        cJSON_AddNumberToObject(value, "nvs_total_bytes", nvs_stats.total_entries * 32);
        cJSON_AddNumberToObject(value, "nvs_namespacbytes", nvs_stats.namespace_count);
    }

    cJSON_AddItemToObject(json, "value", value);

    init_info();

    const char *json_str = cJSON_Print(json);
    if (json_str == NULL) {
        error_log("Failed to create JSON response");
        cJSON_Delete(json);
        return;
    }

    ui_send_json(json_str);
    
    cJSON_free((void *)json_str);
    cJSON_Delete(json);
}

void in_recv_from_ui(cJSON *json_obj) {
    cJSON *eventValue = cJSON_GetObjectItem(json_obj, "event");
    if (eventValue == NULL || !cJSON_IsString(eventValue)) {
        error_log("No event found in JSON or event is not a string");
        return;
    }
    const char *event = eventValue->valuestring;

    if (strcmp(event, "get") == 0) {
        handle_get();
    } else {
        error_log("Received unknown event: %s", event);
    }
}
