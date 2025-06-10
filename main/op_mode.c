#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "config.h"
#include "linked_list.h"
#include "op_mode.h"
#include "rig_controller.h"
#include "ui.h"

static linked_list_t *op_modes;
static SemaphoreHandle_t op_modes_mutex = NULL;

#define TAG "OP_MODE"

static void safe_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    } else {
        ESP_LOGE(TAG, "Attempted to free a NULL pointer");
    }
}

#define MAX_LOG_BUFFER_SIZE 2048
void log_op_mode(char *tag, char *prefix, const op_mode_t *mode) {
    char *buf = (char *)malloc(MAX_LOG_BUFFER_SIZE);
    int len = 0;

    if (prefix != NULL) {
        strcpy(buf, prefix);
        len = strlen(prefix);
    }

    len += sprintf(buf + len, "id: %lu; name: %s; Order: %d", mode->id, mode->name, mode->order);


    len += sprintf(buf + len, "; Frequency Ranges: ");
    int i = 0;
    for (linked_list_node_t *node = linked_list_begin(mode->freq_ranges); node != NULL; node = node->next, i++) {
        freq_range_t *range = (freq_range_t *)node->data;
        if (len >= MAX_LOG_BUFFER_SIZE - 100) {
            len += sprintf(buf + len, "truncating freq_ranges");
            break;
        }
        
        len += sprintf(buf + len, "%sstart=%lu, end=%lu", i == 0 ? "" : ", ", range->start, range->end);
    }

    len += sprintf(buf + len, "; Commands: ");
    if (strlen(mode->commands) > 0) {
        len += sprintf(buf + len, "%s", mode->commands);
    }

    ESP_LOGI(TAG, "%s", buf);
    safe_free(buf);
}

static void free_op_mode(op_mode_t *mode) {
    if (mode == NULL) {
        return;
    }

    linked_list_destroy(mode->freq_ranges, safe_free);

    if (mode->commands != NULL) {
        safe_free(mode->commands);
    }

    safe_free(mode);
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

    cJSON_AddStringToObject(json, "topic", "op_mode");
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

static uint32_t get_next_id() {
    uint32_t next_id;

    if (get_u32("next_op_mode_id", &next_id) != ESP_OK) {
        next_id = 1;
    }

    set_u32("next_op_mode_id", next_id + 1);

    return next_id;
}

static void make_key(char *key, uint32_t id) {
    if (key == NULL) {
        ESP_LOGE(TAG, "Key buffer is NULL");
        return;
    }
    snprintf(key, 11, "%lu", id);
}

static char * encode_op_mode(const op_mode_t *mode) {
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    // id
    cJSON_AddNumberToObject(root, "id", mode->id);

    // name
    cJSON_AddStringToObject(root, "name", mode->name);

    // freq_ranges
    cJSON *freq_ranges = cJSON_CreateArray();
    if (mode->freq_ranges) {
        for (linked_list_node_t *node = linked_list_begin(mode->freq_ranges); node != NULL; node = node->next) {
            freq_range_t *range = (freq_range_t *)node->data;
            if (range && (range->start != 0 || range->end != 0)) {
                cJSON *fr = cJSON_CreateObject();
                cJSON_AddNumberToObject(fr, "start", range->start);
                cJSON_AddNumberToObject(fr, "end", range->end);
                cJSON_AddItemToArray(freq_ranges, fr);
            }
        }
    }
    cJSON_AddItemToObject(root, "freq_ranges", freq_ranges);

    // order
    cJSON_AddNumberToObject(root, "order", mode->order);

    // commands
    cJSON_AddStringToObject(root, "commands", mode->commands);

    // Print to buffer
    char *json_str = cJSON_PrintUnformatted(root);

    cJSON_Delete(root);
    
    // ESP_LOGI(TAG, "Encoded op mode: %s", json_str);
    return json_str;
}

static esp_err_t process_decode_op_mode(cJSON *root, op_mode_t *mode) {
    // id
    cJSON *id = cJSON_GetObjectItem(root, "id");
    if (id == NULL || !cJSON_IsNumber(id)) {
        ESP_LOGE(TAG, "Invalid or missing 'id' in op mode JSON");
        return ESP_FAIL;
    }
    mode->id = (uint32_t)id->valueint;

    // name
    cJSON *name = cJSON_GetObjectItem(root, "name");
    if (name == NULL || !cJSON_IsString(name)) {
        ESP_LOGE(TAG, "Invalid or missing 'name' in op mode JSON");
        return ESP_FAIL;
    }
    strncpy(mode->name, name->valuestring, MAX_OP_MODE_NAME_LEN);

    // freq_ranges
    cJSON *freq_ranges = cJSON_GetObjectItem(root, "freq_ranges");
    if (freq_ranges == NULL || !cJSON_IsArray(freq_ranges)) {
        ESP_LOGE(TAG, "Invalid or missing 'freq_ranges' in op mode JSON");
        return ESP_FAIL;
    }
    
    size_t count = cJSON_GetArraySize(freq_ranges);
    mode->freq_ranges = linked_list_create();
    for (size_t i = 0; i < count; ++i) {
        cJSON *fr = cJSON_GetArrayItem(freq_ranges, i);
        if (fr == NULL || !cJSON_IsObject(fr)) {
            ESP_LOGE(TAG, "Invalid frequency range format in op mode JSON");
            return ESP_FAIL;
        }
        cJSON *start = cJSON_GetObjectItem(fr, "start");
        cJSON *end = cJSON_GetObjectItem(fr, "end");
        if (start == NULL || end == NULL || !cJSON_IsNumber(start) || !cJSON_IsNumber(end)) {
            ESP_LOGE(TAG, "Invalid frequency range format, must contain start and end as numbers");
            return ESP_FAIL;
        }
        freq_range_t *range = (freq_range_t *)calloc(1, sizeof(freq_range_t));
        range->start = start->valuedouble;
        range->end = end->valuedouble;
        linked_list_push(mode->freq_ranges, range);
    }

    // order
    cJSON *order = cJSON_GetObjectItem(root, "order");
    if (order == NULL || !cJSON_IsNumber(order)) {
        ESP_LOGE(TAG, "Invalid or missing 'order' in op mode JSON");
        return ESP_FAIL;
    }
    mode->order = order->valueint;

    // commands
    cJSON *commands = cJSON_GetObjectItem(root, "commands");
    if (commands == NULL || !cJSON_IsString(commands)) {
        ESP_LOGE(TAG, "Invalid or missing 'commands' in op mode JSON");
        return ESP_FAIL;
    }
    mode->commands = strdup(commands->valuestring);

    return ESP_OK;
}

static op_mode_t *decode_op_mode(const uint8_t *json, size_t json_size) {
    if (json == NULL || json_size == 0) {
        return NULL;
    }

    cJSON *root = cJSON_ParseWithLength((const char *)json, json_size);
    if (!root) {
        return NULL;
    }

    op_mode_t *mode = (op_mode_t *)calloc(1, sizeof(op_mode_t));
    if (!mode) {
        cJSON_Delete(root);
        return NULL;
    }

    if (process_decode_op_mode(root, mode) != ESP_OK) {
        free_op_mode(mode);
        cJSON_Delete(root);
        return NULL;
    }

    cJSON_Delete(root);

    return mode;
}

static bool op_mode_exists(const char *name) {
    if (name == NULL) {
        ESP_LOGE(TAG, "Op mode name is NULL");
        return false;
    }

    for (linked_list_node_t *omn = linked_list_begin(op_modes); omn != NULL; omn = linked_list_next(omn)) {
        op_mode_t *op_mode = (op_mode_t *)omn->data;
        if (strcmp(op_mode->name, name) == 0) {
            return true;
        }
    }
    return false;
}

static op_mode_t *get_op_mode(const uint32_t id) {
    for (linked_list_node_t *node = linked_list_begin(op_modes); node != NULL; node = linked_list_next(node)) {
        op_mode_t *op_mode = (op_mode_t *)node->data;
        if (op_mode != NULL && op_mode->id == id) {
            return op_mode;
        }
    }
    return NULL;
}

static esp_err_t load_op_mode(nvs_open_mode_t nvs_handle, const char *key) {
    size_t required_size = 0;
    esp_err_t err = nvs_get_str(nvs_handle, key, NULL, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read op mode size from NVS: %s", esp_err_to_name(err));
        return ESP_FAIL;
    }

    char *buffer = malloc(required_size);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for op mode buffer: %d", required_size);
        return ESP_FAIL;
    }

    err = nvs_get_str(nvs_handle, key, buffer, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read op mode from NVS: %s", esp_err_to_name(err));
        safe_free(buffer);
        return ESP_FAIL;
    }

    op_mode_t *mode = decode_op_mode((const uint8_t *)buffer, required_size);
    safe_free(buffer);
    if (mode == NULL) {
        ESP_LOGE(TAG, "Failed to decode op mode from JSON");
        return ESP_FAIL;
    }
    
    linked_list_push(op_modes, mode);

    // log_op_mode(TAG, "Loaded op Mode: ", mode);
    return ESP_OK;
}

static void handle_load() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(OP_MODE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "handle_load(): failed to open NVS for op modes: %s", esp_err_to_name(err));
        return;
    }
    
    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find("nvs", OP_MODE_NAMESPACE, NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);

        if (load_op_mode(nvs_handle, info.key) != ESP_OK) {
            nvs_release_iterator(it);
            nvs_close(nvs_handle);
            return;
        }

        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);

    rt_find_op_node_by_freq();
}

static esp_err_t save_to_nvs(op_mode_t *mode) {
    if (mode == NULL) {
        return ESP_FAIL;
    }

    if (mode->id == 0) {
        mode->id = get_next_id();
    }
    ESP_LOGI(TAG, "Persisting op mode %lu", mode->id);

    char *om_str = encode_op_mode(mode);
    if (om_str == NULL) {
        ESP_LOGE(TAG, "Failed to encode op mode");
        return ESP_FAIL;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(OP_MODE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS for op modes: %s", esp_err_to_name(err));
        safe_free(om_str);
        return err;
    }

    char key[11];
    make_key(key, mode->id);
    ESP_LOGI(TAG, "Writing op mode '%s' to NVS with key '%s'", mode->name, key);

    log_op_mode(TAG, "Persisting op Mode: ", mode);
    err = nvs_set_str(nvs_handle, key, om_str);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write op mode to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        safe_free(om_str);
        return err;
    }

    if (nvs_commit(nvs_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit op mode to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        safe_free(om_str);
        return err;
    }

    nvs_close(nvs_handle);
    safe_free(om_str);

    ESP_LOGI(TAG, "Op mode '%lu' persisted successfully", mode->id);

    return ESP_OK;
}

static void remove_from_nvs(op_mode_t *mode) {
    if (mode == NULL || mode->id == 0) {
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(OP_MODE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        error_log("remove_from_nvs(): failed to open NVS for op modes: %s", esp_err_to_name(err));
        return;
    }

    char key[11];
    make_key(key, mode->id);
    ESP_LOGI(TAG, "Removing op mode '%s' from NVS with key '%s'", mode->name, key);

    err = nvs_erase_key(nvs_handle, key);
    if (err != ESP_OK) {
        error_log("Failed to erase op mode from NVS: %s", esp_err_to_name(err));
        return;
    }

    nvs_close(nvs_handle);

    ESP_LOGI(TAG, "Op mode '%lu' removed successfully", mode->id);
}

static void send_op_mode(const op_mode_t *mode) {
    if (mode == NULL) {
        error_log("Op mode is NULL");
        return;
    }

    char *om_str = encode_op_mode(mode);
    if (om_str == NULL) {
        error_log("Failed to encode op mode");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        error_log("Failed to create JSON object");
        safe_free(om_str);
        return;
    }

    cJSON_AddStringToObject(json, "topic", "op_mode");
    cJSON_AddStringToObject(json, "event", "update");
    cJSON_AddRawToObject(json, "value", om_str);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str != NULL) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        error_log("Failed to print op mode JSON");
    }

    safe_free(om_str);

    cJSON_Delete(json);
}

static void send_deleted(const op_mode_t *mode) {
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        error_log("Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "op_mode");
    cJSON_AddStringToObject(json, "event", "removed");
    cJSON_AddNumberToObject(json, "value", mode->id);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str) {
        ESP_LOGI(TAG, "Sending op mode: %s", json_str);
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        error_log("Failed to print op mode JSON");
    }

    cJSON_Delete(json);
}

static void create_op_mode(op_mode_t *mode) {
    if (op_mode_exists(mode->name)) {
        error_log("Op mode already exists: %s", mode->name);
        free_op_mode(mode);
        return;
    }

    if (mode->id != 0) {
        error_log("Op mode ID should be 0 for new modes, got: %lu", mode->id);
        free_op_mode(mode);
        return;
    }

    if (save_to_nvs(mode) != ESP_OK) {
        error_log("Failed to persist op mode");
        free_op_mode(mode);
        return;
    }

    linked_list_push(op_modes, mode);

    log_op_mode(TAG, "Created op Mode: ", mode);
    send_op_mode(mode);

    rt_find_op_node_by_freq();
}

void send_config() {
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "op_mode");
    cJSON_AddStringToObject(json, "event", "config");
    
    cJSON *config = cJSON_CreateObject();
    if (!config) {
        cJSON_Delete(json);
        return;
    }
    cJSON_AddNumberToObject(config, "MAX_OP_MODE_NAME_LEN", MAX_OP_MODE_NAME_LEN);
    cJSON_AddNumberToObject(config, "MAX_ORDER", MAX_ORDER);
    cJSON_AddItemToObject(json, "value", config);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str) {
        // ESP_LOGI(TAG, "Config: %s", json_str);
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "Failed to print JSON");
    }

    cJSON_Delete(json);
}

static void send_refresh() {
    ui_send_json("{\"topic\":\"op_mode\",\"event\":\"refresh\",\"value\":\"start\"}");

    for (linked_list_node_t *node = linked_list_begin(op_modes); node != NULL; node = linked_list_next(node)) {
        send_op_mode((op_mode_t *)node->data);
    }

    ui_send_json("{\"topic\":\"op_mode\",\"event\":\"refresh\",\"value\":\"complete\"}");
}

static void remove_op_mode(uint32_t id) {
    if (id == 0) {
        error_log("Op mode ID 0 cannot be removed");
        return;
    }

    op_mode_t *mode = get_op_mode(id);
    if (mode == NULL) {
        error_log("Op mode not found: %lu", id);
        return;
    }

    log_op_mode(TAG, "Removing op Mode: ", mode);

    remove_from_nvs(mode);
    send_deleted(mode);

    linked_list_remove(op_modes, mode);
    free_op_mode(mode);

    rt_find_op_node_by_freq();

    return;
}

static void update_op_mode(op_mode_t *mode) {
    if (mode == NULL) {
        error_log("Op mode is NULL");
        return;
    }

    if (mode->id == 0) {
        error_log("Op mode ID 0 cannot be updated");
        return;
    }

    op_mode_t *old_mode = get_op_mode(mode->id);
    if (old_mode == NULL) {
        error_log("Op mode not found: %lu", mode->id);
        return;
    }
    
    if (save_to_nvs(mode) != ESP_OK) {
        error_log("Failed to persist op mode");
        free_op_mode(mode);
        return;
    }

    free_op_mode(old_mode);
    linked_list_remove(op_modes, old_mode);
    linked_list_push(op_modes, mode);

    log_op_mode(TAG, "Updated op Mode: ", mode);
    send_op_mode(mode);

    rt_find_op_node_by_freq();
}

static void handle_ui_event(const char *event, cJSON *json_obj) {
    if (strcmp(event, "refresh") == 0) {
        send_refresh();
        send_config();
        return;
    }
    
    cJSON *value_obj = cJSON_GetObjectItem(json_obj, "value");
    if (value_obj == NULL || !cJSON_IsObject(value_obj)) {
        error_log("Missing or invalid value parameter");
        return;
    }
    
    cJSON *idValue = cJSON_GetObjectItem(value_obj, "id");
    if (idValue == NULL || !cJSON_IsNumber(idValue)) {
        error_log("Missing or invalid id parameter");
        return;
    }
    uint32_t id = (uint32_t)idValue->valueint;
    
    if (strcmp(event, "remove") == 0) {
        remove_op_mode(id);
        return;
    }

    op_mode_t *mode = (op_mode_t *)calloc(1, sizeof(op_mode_t));
    if (!mode) {
        error_log("Failed to allocate memory for op mode");
        return;
    }

    if (process_decode_op_mode(value_obj, mode) != ESP_OK) {
        free_op_mode(mode);
        return;
    }

    if (strcmp(event, "create") == 0) {
        create_op_mode(mode);
        return;
    }

    if (strcmp(event, "update") == 0) {
        update_op_mode(mode);
        return;
    }

    free_op_mode(mode);
    error_log("error", "Unknown op mode event: %s", event);
}

void om_recv_from_ui(cJSON *json_obj) {
    cJSON *eventValue = cJSON_GetObjectItem(json_obj, "event");
    if (eventValue == NULL || !cJSON_IsString(eventValue)) {
        error_log("Missing or invalid type in JSON");
        return;
    }
    char *event = eventValue->valuestring;
    ESP_LOGI(TAG, "Received event: %s", event);

    if (event == NULL || strlen(event) == 0) {
        error_log("Missing or invalid event string");
        return;
    }

    if (xSemaphoreTake(op_modes_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        error_log("Failed to take op modes mutex");
        return;
    }
    
    handle_ui_event(event, json_obj);
    
    xSemaphoreGive(op_modes_mutex);
}

esp_err_t om_find_by_frequency_in_mutex(uint32_t frequency, char **name, char **commands) {

    // TODO: Manual mode
    
    op_mode_t *candidate = NULL;
    for (linked_list_node_t *omn = linked_list_begin(op_modes); omn != NULL; omn = linked_list_next(omn)) {
        op_mode_t *mode = (op_mode_t *)omn->data;

        for (linked_list_node_t *frn = linked_list_begin(mode->freq_ranges); frn != NULL; frn = frn->next) {
            freq_range_t *range = (freq_range_t *)frn->data;

            if (range->start <= frequency && frequency <= range->end) {
                if (candidate == NULL || mode->order < candidate->order) {
                    candidate = mode;
                }
                break;
            }
        }
    }

    if (candidate == NULL) {
        ESP_LOGE(TAG, "No op mode found for frequency %lu, using default", frequency);
        return ESP_FAIL;
    }
    
    *name = strdup(candidate->name);
    if (*name == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for op mode name");
        return ESP_FAIL;
    }

    *commands = strdup(candidate->commands);
    if (*commands == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for op mode commands");
        safe_free(*name);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t om_find_by_frequency(uint32_t frequency, char **name, char **commands) {
    if (xSemaphoreTake(op_modes_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        error_log("update_op_mode(): Failed to take op modes mutex");
        return ESP_FAIL;
    }
    
    esp_err_t err = om_find_by_frequency_in_mutex(frequency, name, commands);

    xSemaphoreGive(op_modes_mutex);
    return err;
}

esp_err_t om_find_by_name_in_mutex(const char *name, char **commands) {
    if (name == NULL || strlen(name) == 0) {
        ESP_LOGE(TAG, "Invalid op mode name");
        return ESP_FAIL;
    }

    for (linked_list_node_t *omn = linked_list_begin(op_modes); omn != NULL; omn = linked_list_next(omn)) {
        op_mode_t *mode = (op_mode_t *)omn->data;

        if (strcmp(mode->name, name) == 0) {
            *commands = strdup(mode->commands);
            if (*commands == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for op mode commands");
                return ESP_FAIL;
            }

            return ESP_OK;
        }
    }

    return ESP_FAIL;
}

esp_err_t om_find_by_name(const char *name, char **commands) {
    if (xSemaphoreTake(op_modes_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        error_log("Failed to take op modes mutex");
        return ESP_FAIL;
    }

    esp_err_t err = om_find_by_name_in_mutex(name, commands);

    xSemaphoreGive(op_modes_mutex);
    return err;
}

static op_mode_t *create_default_op_mode() {
    op_mode_t *mode = (op_mode_t *)calloc(1, sizeof(op_mode_t));
    if (mode == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for default op mode");
        return NULL;
    }

    mode->id = 0;
    strncpy(mode->name, "Default", MAX_OP_MODE_NAME_LEN);
    mode->order = MAX_ORDER;

    freq_range_t *default_range = (freq_range_t *)calloc(1, sizeof(freq_range_t));
    default_range->start = 0;
    default_range->end = 999999999;

    mode->freq_ranges = linked_list_create();
    linked_list_push(mode->freq_ranges, default_range);

    mode->commands = strdup("");

    return mode;
}

void init_op_mode() {
    op_modes_mutex = xSemaphoreCreateMutex();

    op_modes = linked_list_create();
    if (op_modes == NULL) {
        ESP_LOGE(TAG, "Failed to create linked list for op modes");
        return;
    }

    linked_list_push(op_modes, create_default_op_mode());
    handle_load();

    ESP_LOGI(TAG, "Op mode initialized");
}
