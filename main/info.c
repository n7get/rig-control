#include "cJSON.h"
#include "config.h"
#include "esp_log.h"
#include "http.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"
#include "rig_commands.h"

#define TAG "INFO"

extern rig_command_t rig_commands[];

info_t info;

info_t *get_info() {
    return &info;
}

static void init_info() {
    info.no_empty_sendqueue = 0;
    info.no_busy_sendqueue = 0;
    info.no_sendqueue_waiting = 0;
    info.total_sendqueue = 0;
    info.max_send_len = 0;
    info.max_receive_len = 0;
    info.total_response_time = 0;
    info.no_responses = 0;
    info.max_response_time = 0;
    info.polls = 0;
    info.elapsed_ticks = 0;
    info.pending = 0;
    info.valid = 0;
    info.updates = 0;
    info.send_queue_full = 0;
    info.uart_max_read_len = 0;
    info.uart_buffer_full = 0;
    info.uart_fifo_ovf = 0;
    info.input_queue_full = 0;
}

static esp_err_t get_info_handler(httpd_req_t *req) {
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        return ESP_FAIL;
    }

    cJSON_AddNumberToObject(json, "no_empty_sendqueue", info.no_empty_sendqueue);
    cJSON_AddNumberToObject(json, "no_busy_sendqueue", info.no_busy_sendqueue);
    float busy_sendqueue_percent = ((float)info.no_busy_sendqueue / (float)(info.no_empty_sendqueue + info.no_busy_sendqueue)) * 100.0f;
    cJSON_AddNumberToObject(json, "busy_sendqueue_percent", busy_sendqueue_percent);
    cJSON_AddNumberToObject(json, "no_sendqueue_waiting", info.no_sendqueue_waiting);
    cJSON_AddNumberToObject(json, "total_sendqueue", info.total_sendqueue);
    float avg_sendqueue_len = (float)info.no_sendqueue_waiting / (float)info.total_sendqueue;
    cJSON_AddNumberToObject(json, "avg_sendqueue_len", avg_sendqueue_len);

    cJSON_AddNumberToObject(json, "max_send_len", info.max_send_len);
    cJSON_AddNumberToObject(json, "max_receive_len", info.max_receive_len);
    cJSON_AddNumberToObject(json, "total_response_time", info.total_response_time / 1000);
    cJSON_AddNumberToObject(json, "no_responses", info.no_responses);
    cJSON_AddNumberToObject(json, "max_response_time", info.max_response_time / 1000);
    float avg_response_time = (float)info.total_response_time / (float)info.no_responses;
    cJSON_AddNumberToObject(json, "avg_response_time", avg_response_time / 1000.0f);

    cJSON_AddNumberToObject(json, "polls", info.polls);
    float avg_elapsed_ticks = (float)info.elapsed_ticks / (float)info.polls;
    cJSON_AddNumberToObject(json, "avg_elapsed_ticks", avg_elapsed_ticks);
    float avg_pending = (float)info.pending / (float)info.polls;
    cJSON_AddNumberToObject(json, "avg_pending", avg_pending);
    float avg_valid = (float)info.valid / (float)info.polls;
    cJSON_AddNumberToObject(json, "avg_valid", avg_valid);
    float avg_updates = (float)info.updates / (float)info.polls;
    cJSON_AddNumberToObject(json, "avg_updates", avg_updates);
    cJSON_AddNumberToObject(json, "send_queue_full", info.send_queue_full);
    cJSON_AddNumberToObject(json, "uart_max_read_len", info.uart_max_read_len);
    cJSON_AddNumberToObject(json, "uart_buffer_full", info.uart_buffer_full);
    cJSON_AddNumberToObject(json, "uart_fifo_ovf", info.uart_fifo_ovf);
    cJSON_AddNumberToObject(json, "input_queue_full", info.input_queue_full);

    cJSON_AddNumberToObject(json, "free_heap", esp_get_free_heap_size());

    init_info();

    const char *json_str = cJSON_Print(json);
    if (json_str == NULL) {
        cJSON_Delete(json);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, json_str);

    cJSON_Delete(json);
    return ESP_OK;
}

void register_info_endpoints(void) {
    register_html_page("/api/info", HTTP_GET, get_info_handler);

    init_info();

    ESP_LOGI(TAG, "Info endpoint registered");
}
