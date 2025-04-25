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

    cJSON_AddNumberToObject(json, "free_heap", esp_get_free_heap_size());

    // add rig_commands to json
    // cJSON *commands_array = cJSON_CreateArray();
    // if (commands_array == NULL) {
    //     cJSON_Delete(json);
    //     return ESP_FAIL;
    // }
    // cJSON_AddItemToObject(json, "rig_commands", commands_array);
    // for (int i = 0; rig_commands[i].cmd != NULL; i++) {
    //     cJSON *command_obj = cJSON_CreateObject();
    //     if (command_obj == NULL) {
    //         cJSON_Delete(json);
    //         return ESP_FAIL;
    //     }
    //     cJSON_AddStringToObject(command_obj, "cmd", rig_commands[i].cmd);
    //     // cJSON_AddNumberToObject(command_obj, "len", rig_commands[i].len);
    //     cJSON_AddBoolToObject(command_obj, "pending", rig_commands[i].pending);
    //     cJSON_AddNumberToObject(command_obj, "refresh_time", rig_commands[i].refresh_time);
    //     cJSON_AddNumberToObject(command_obj, "next_refresh", rig_commands[i].next_refresh);
    //     cJSON_AddItemToArray(commands_array, command_obj);
    // }

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
