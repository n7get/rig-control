#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "config.h"
#include "linked_list.h"
#include "op_mode.h"
#include "rig_commands.h"
#include "rig_monitor.h"
#include "ui.h"

typedef enum {
    OM_LOAD = 0,
    OM_CREATE,
    OM_REMOVE,
    OM_UPDATE,
    OM_REFRESH,
    OM_SET_CURRENT,
    OM_FREQ_CHANGE,
    OM_NEXT_COMMAND,
    OM_READY,
    OM_NOT_READY,
} om_event_type_t;

typedef struct {
    om_event_type_t type;
    uint32_t id;
    char name[MAX_OP_MODE_NAME_LEN + 1];
    linked_list_t *freq_ranges;
    uint8_t order;
    char *commands; // malloced
    uint32_t frequency;
} om_event_t;

static QueueHandle_t om_event_queue;
static linked_list_t *op_modes;
static op_mode_t *current_op_mode = NULL;
static op_mode_t *default_op_mode = NULL;
static uint32_t current_frequency = 999999999;
static linked_list_t *commands_list = NULL;
static char last_command[SEND_BUFFER_SIZE] = {0};

static void set_current_op_mode(op_mode_t *mode);

#define TAG "OP_MODE"

static void safe_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    } else {
        ESP_LOGE(TAG, "Attempted to free a NULL pointer");
    }
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

static void free_om_event(om_event_t *om_event) {
    if (om_event == NULL) {
        return;
    }

    linked_list_destroy(om_event->freq_ranges, safe_free);

    if (om_event->commands) {
        safe_free(om_event->commands);
    }

    safe_free(om_event);
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

static void send_current_op_mode() {
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "op_mode");
    cJSON_AddStringToObject(json, "event", "current");
    cJSON_AddNumberToObject(json, "value", current_op_mode->id);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "Failed to print JSON");
    }

    cJSON_Delete(json);
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

    for (linked_list_node_t *lli = linked_list_begin(op_modes); lli != NULL; lli = linked_list_next(lli)) {
        op_mode_t *op_mode = (op_mode_t *)lli->data;
        if (op_mode != NULL && strcmp(op_mode->name, name) == 0) {
            return true;
        }
    }
    return false;
}

static op_mode_t *get_op_mode(const uint32_t id) {
    if (id == 0) {
        return default_op_mode;
    }

    for (linked_list_node_t *node = linked_list_begin(op_modes); node != NULL; node = linked_list_next(node)) {
        op_mode_t *op_mode = (op_mode_t *)node->data;
        if (op_mode != NULL && op_mode->id == id) {
            return op_mode;
        }
    }
    return NULL;
}

static esp_err_t parse_value(const cJSON *json_obj, om_event_t *om_event) {
    cJSON *nameValue = cJSON_GetObjectItem(json_obj, "name");
    if (nameValue == NULL || !cJSON_IsString(nameValue)) {
        error_log("Missing or invalid name parameter");
        return ESP_FAIL;
    }
    strncpy(om_event->name, nameValue->valuestring, MAX_OP_MODE_NAME_LEN);

    cJSON *order = cJSON_GetObjectItem(json_obj, "order");
    if (order == NULL || !cJSON_IsNumber(order)) {
        error_log("Missing or invalid order parameter");
        return ESP_FAIL;
    }
    if (order->valueint < 0 || order->valueint > MAX_ORDER) {
        error_log("Order value out of range");
        return ESP_FAIL;
    }
    om_event->order = (uint8_t)order->valueint;

    // frequency ranges
    cJSON *freq_ranges = cJSON_GetObjectItem(json_obj, "freq_ranges");
    if (freq_ranges == NULL || !cJSON_IsArray(freq_ranges)) {
        error_log("Missing or invalid frequency ranges array");
        return ESP_FAIL;
    }

    size_t count = cJSON_GetArraySize(freq_ranges);
    om_event->freq_ranges = linked_list_create();
    for (size_t i = 0; i < count; ++i) {
        cJSON *fr = cJSON_GetArrayItem(freq_ranges, i);
        if (fr == NULL || !cJSON_IsObject(fr)) {
            error_log("Invalid frequency range format in JSON");
            return ESP_FAIL;
        }
        cJSON *start = cJSON_GetObjectItem(fr, "start");
        cJSON *end = cJSON_GetObjectItem(fr, "end");
        if (start == NULL || end == NULL || !cJSON_IsNumber(start) || !cJSON_IsNumber(end)) {
            error_log("Invalid frequency range format, must contain start and end as numbers");
            return ESP_FAIL;
        }
        freq_range_t *range = (freq_range_t *)calloc(1, sizeof(freq_range_t));
        range->start = start->valuedouble;
        range->end = end->valuedouble;
        linked_list_push(om_event->freq_ranges, range);
    }

    cJSON *commands = cJSON_GetObjectItem(json_obj, "commands");
    if (commands == NULL || !cJSON_IsString(commands)) {
        error_log("Missing or invalid commands parameter");
        return ESP_FAIL;
    }

    char *commands_str = commands->valuestring;
    if (commands_str == NULL || !cJSON_IsString(commands)) {
        error_log("Missing or invalid commands string");
        return ESP_FAIL;
    }

    om_event->commands = strdup(commands_str);

    return ESP_OK;
}

static esp_err_t create_om_event(cJSON *json_obj, om_event_t *om_event) {
    cJSON *eventValue = cJSON_GetObjectItem(json_obj, "event");
    if (eventValue == NULL || !cJSON_IsString(eventValue)) {
        error_log("Missing or invalid type in JSON");
        return ESP_FAIL;
    }
    char *event = eventValue->valuestring;
    ESP_LOGI(TAG, "Received event: %s", event);

    if (event == NULL || strlen(event) == 0) {
        error_log("Invalid event string");
        return ESP_FAIL;
    }

    if (strcmp(event, "refresh") == 0) {
        om_event->type = OM_REFRESH;
        return ESP_OK;
    }

    cJSON *value_obj = cJSON_GetObjectItem(json_obj, "value");
    if (value_obj == NULL || !cJSON_IsObject(value_obj)) {
        error_log("No value object found in JSON for event: %s", event);
        return ESP_FAIL;
    }

    cJSON *idValue = cJSON_GetObjectItem(value_obj, "id");
    if (idValue == NULL || !cJSON_IsNumber(idValue)) {
        error_log("Missing or invalid id parameter");
        return ESP_FAIL;
    }
    om_event->id = (uint32_t)idValue->valueint;

    if (strcmp(event, "remove") == 0) {
        om_event->type = OM_REMOVE;
        return ESP_OK;
    }
    if (strcmp(event, "set_current") == 0) {
        om_event->type = OM_SET_CURRENT;
        return ESP_OK;
    }

    if (parse_value(value_obj, om_event) != ESP_OK) {
        return ESP_FAIL;
    }

    if (strcmp(event, "create") == 0) {
        om_event->type = OM_CREATE;
        return ESP_OK;
    }
    if (strcmp(event, "update") == 0) {
        om_event->type = OM_UPDATE;
        return ESP_OK;
    }

    error_log("error", "Unknown op mode event: %s", event);
    return ESP_FAIL;
}

static char *find_priority_command(bool (*match)(const char *value)) {
    for (linked_list_node_t *lli = linked_list_begin(commands_list); lli != NULL; lli = linked_list_next(lli)) {
        char *cmd = (char *)lli->data;
        if (match(cmd)) {
            linked_list_remove(commands_list, cmd);
            return cmd;
        }
    }
    return NULL;
}

static void send_next_command() {
    if (linked_list_size(commands_list) == 0) {
        ESP_LOGI(TAG, "No commands left for current op mode");
        return;
    }

    char *cmd = find_priority_command(rc_is_mode_command);
    if (cmd == NULL) {
        cmd = find_priority_command(rc_is_narrow_command);
    }
    if (cmd == NULL) {
        cmd = find_priority_command(rc_is_width_command);
    }
    if (cmd == NULL) {
        cmd = linked_list_pop(commands_list);
    }

    if (cmd == NULL) {
        ESP_LOGW(TAG, "No command found in current op mode");
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

static void find_op_node_by_freq() {

    // TODO: Manual mode

    op_mode_t *candidate = NULL;

    for (linked_list_node_t *lli = linked_list_begin(op_modes); lli != NULL; lli = linked_list_next(lli)) {
        op_mode_t *mode = (op_mode_t *)lli->data;

        if (mode != NULL) {
            if (mode->freq_ranges) {
                for (linked_list_node_t *node = linked_list_begin(mode->freq_ranges); node != NULL; node = node->next) {
                    freq_range_t *range = (freq_range_t *)node->data;
                    if (range && range->start != 0 && range->end != 0) {
                        if (range->start <= current_frequency && current_frequency <= range->end) {
                            if (candidate == NULL || mode->order < candidate->order) {
                                candidate = mode;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    if (candidate == NULL) {
        ESP_LOGI(TAG, "No op mode found for frequency %lu, using default", current_frequency);
        candidate = default_op_mode;
    }

    if (candidate != current_op_mode) {
        ESP_LOGI(TAG, "New Op Mode found for frequency %lu: %s", current_frequency, candidate->name);
        set_current_op_mode(candidate);
    }
}

static void set_current_op_mode(op_mode_t *mode) {
    if (mode == NULL) {
        ESP_LOGE(TAG, "Cannot set current op mode to NULL");
        return;
    }

    if (current_op_mode != NULL && current_op_mode->id == mode->id) {
        ESP_LOGI(TAG, "Current op mode is already set to: %s", current_op_mode->name);
        return;
    }

    current_op_mode = mode;
    send_current_op_mode();

    linked_list_clear(commands_list, safe_free);

    char *token, *string, *tofree;
    tofree = string = strdup(mode->commands);
    if (string == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for command string");
        return;
    }

    // iterate through all commands in the mode and add them to the linked list
    while ((token = strsep(&string, ";")) != NULL) {
        if (strlen(token) > 0) {
            char *data = malloc(strlen(token) + 2);
            if (data == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for command data");
                linked_list_clear(commands_list, safe_free);
                safe_free(tofree);
                return;
            }
            strcpy(data, token);
            strcat(data, ";"); // Ensure command ends with a semicolon

            linked_list_push(commands_list, data);
        }
    }
    safe_free(tofree);

    if (linked_list_size(commands_list) == 0) {
        ESP_LOGE(TAG, "No commands found in op mode: %s", mode->name);
        return;
    }

    send_next_command();
}

void om_recv_from_ui(cJSON *json_obj) {
    om_event_t *om_event = calloc(1, sizeof(om_event_t));
    if (om_event == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for op mode event");
        return;
    }

    if (create_om_event(json_obj, om_event) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create op mode event from JSON");
        free_om_event(om_event);
        return;
    }

    if (xQueueSend(om_event_queue, &om_event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue op event");
        free_om_event(om_event);
    }
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

    find_op_node_by_freq();
}

static esp_err_t op_mode_persist(op_mode_t *mode) {
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
        ESP_LOGE(TAG, "op_mode_persist(): failed to open NVS for op modes: %s", esp_err_to_name(err));
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

    send_op_mode(mode);
    ESP_LOGI(TAG, "Op mode '%lu' persisted successfully", mode->id);

    return ESP_OK;
}

static void op_mode_remove(op_mode_t *mode) {
    if (mode == NULL || mode->id == 0) {
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(OP_MODE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        error_log("op_mode_remove(): failed to open NVS for op modes: %s", esp_err_to_name(err));
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

static void handle_create(om_event_t *om_event) {
    if (op_mode_exists(om_event->name)) {
        error_log("Op mode already exists: %s", om_event->name);
        return;
    }

    if (om_event->id != 0) {
        error_log("Op mode ID should be 0 for new modes, got: %lu", om_event->id);
        return;
    }

    op_mode_t *mode = (op_mode_t *)calloc(1, sizeof(op_mode_t));
    if (mode == NULL) {
        error_log("Failed to allocate memory for op mode");
        return;
    }

    strncpy(mode->name, om_event->name, MAX_OP_MODE_NAME_LEN);
    mode->order = om_event->order;

    mode->freq_ranges = om_event->freq_ranges;
    om_event->freq_ranges = NULL;

    mode->commands = om_event->commands ? strdup(om_event->commands) : NULL;

    if (op_mode_persist(mode) != ESP_OK) {
        error_log("Failed to persist op mode");
        free_op_mode(mode);
        return;
    }

    linked_list_push(op_modes, mode);
    
    log_op_mode(TAG, "Created op Mode: ", mode);

    find_op_node_by_freq();
}

static void send_refresh() {
    ui_send_json("{\"topic\":\"op_mode\",\"event\":\"refresh\",\"value\":\"start\"}");

    send_op_mode(default_op_mode);
    for (linked_list_node_t *lli = linked_list_begin(op_modes); lli != NULL; lli = linked_list_next(lli)) {
        send_op_mode((op_mode_t *)lli->data);
    }

    send_current_op_mode();
    ui_send_json("{\"topic\":\"op_mode\",\"event\":\"refresh\",\"value\":\"complete\"}");
}

static void handle_remove(const om_event_t *om_event) {
    if (om_event->id == 0) {
        error_log("Op mode ID 0 cannot be removed");
        return;
    }

    op_mode_t *mode = get_op_mode(om_event->id);
    if (mode == NULL) {
        error_log("Op mode not found: %lu", om_event->id);
        return;
    }

    log_op_mode(TAG, "Removing op Mode: ", mode);

    op_mode_remove(mode);
    send_deleted(mode);

    linked_list_remove(op_modes, mode);
    free_op_mode(mode);

    find_op_node_by_freq();

    return;
}

static void handle_update(om_event_t *om_event) {
    if (om_event->id == 0) {
        error_log("Op mode ID 0 cannot be updated");
        return;
    }

    op_mode_t *mode = get_op_mode(om_event->id);
    if (mode == NULL) {
        error_log("Op mode not found: %lu", om_event->id);
        return;
    }

    memcpy(mode->name, om_event->name, MAX_OP_MODE_NAME_LEN);
    mode->name[MAX_OP_MODE_NAME_LEN - 1] = '\0'; // Ensure null termination
    mode->order = om_event->order;

    linked_list_destroy(mode->freq_ranges, safe_free);
    mode->freq_ranges = om_event->freq_ranges;
    om_event->freq_ranges = NULL;
    
    if (mode->commands) {
        safe_free(mode->commands);
    }
    mode->commands = om_event->commands;
    om_event->commands = NULL;

    if (op_mode_persist(mode) != ESP_OK) {
        error_log("Failed to persist op mode");
        return;
    }

    find_op_node_by_freq();
}

static void handle_set_current(const om_event_t *om_event) {
    op_mode_t *mode = get_op_mode(om_event->id);
    if (mode == NULL) {
        error_log("Op mode not found: %lu", om_event->id);
        return;
    }
    
    ESP_LOGI(TAG, "Setting current op mode: %lu", mode->id);
    set_current_op_mode(mode);
}

static void handle_freq_change(uint32_t frequency) {
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

    find_op_node_by_freq();
}

static void handle_next_command() {
    last_command[0] = '\0';
    send_next_command();
}

static void op_mode_task(void *pvParameters) {
    bool is_ready = false;

    while(1) {
        om_event_t *om_event;

        if (xQueueReceive(om_event_queue, &om_event, portMAX_DELAY) == pdPASS) {
            switch(om_event->type) {
            case OM_LOAD:
                handle_load();
                break;
            case OM_CREATE:
                handle_create(om_event);
                break;
            case OM_REMOVE:
                handle_remove(om_event);
                break;
            case OM_UPDATE:
                handle_update(om_event);
                break;
            case OM_REFRESH:
                send_refresh();
                send_config();
                break;
            case OM_SET_CURRENT:
                handle_set_current(om_event);
                break;
            case OM_FREQ_CHANGE:
                if (is_ready) {
                    handle_freq_change(om_event->frequency);
                }
                break;
            case OM_NEXT_COMMAND:
                handle_next_command();
                break;
            case OM_READY:
                is_ready = true;
                break;
            case OM_NOT_READY:
                is_ready = false;
                break;
            default:
                error_log("Unknown op mode event type: %d", om_event->type);
                break;
            }
        }
        free_om_event(om_event);
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
    if (mode->freq_ranges) {
        for (linked_list_node_t *node = linked_list_begin(mode->freq_ranges); node != NULL; node = node->next, ++i) {
            freq_range_t *range = (freq_range_t *)node->data;
            if (len >= MAX_LOG_BUFFER_SIZE - 100) {
                len += sprintf(buf + len, "truncating freq_ranges");
                break;
            }
            if (range && (range->start != 0 || range->end != 0)) {
                len += sprintf(buf + len, "%sstart=%lu, end=%lu", i == 0 ? "" : ", ", range->start, range->end);
            }
        }
    }

    len += sprintf(buf + len, "; Commands: ");
    if (strlen(mode->commands) > 0) {
        len += sprintf(buf + len, "%s", mode->commands);
    }

    ESP_LOGI(TAG, "%s", buf);
    safe_free(buf);
}

static void send_event(int type) {
    om_event_t *om_event = (om_event_t *)calloc(1, sizeof(om_event_t));
    om_event->type = type;

    if (xQueueSend(om_event_queue, &om_event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue op event");
    }
}

static void status_notification_handler(void *context, void *data) {
    ESP_LOGI(TAG, "Received status notification: %s", (char *)data);
    if (strcmp((char *)data, "ready") == 0) {
        send_event(OM_READY);

        if (rm_queue_command(rc_freq_command(), SEND_TYPE_READ) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to queue get frequency command");
            return;
        }
    } else if (strcmp((char *)data, "not_ready") == 0) {
        send_event(OM_NOT_READY);
    }
}

static void send_frequency_change_event(uint32_t frequency) {
    om_event_t *om_event = (om_event_t *)calloc(1, sizeof(om_event_t));
    om_event->type = OM_FREQ_CHANGE;
    om_event->frequency = frequency;

    if (xQueueSend(om_event_queue, &om_event, pdMS_TO_TICKS(RESPONSE_TIMEOUT_MS)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue frequency change event");
    }
}

static void command_notification_handler(void *context, void *data) {
    ESP_LOGI(TAG, "Received command notification: %s", (char *)data);
    if (rc_commands_are_same(last_command, data)) {
        send_event(OM_NEXT_COMMAND);
    }

    uint32_t freq = rc_parse_frequency((const char *)data);
    if (freq != 0) {
        send_frequency_change_event(freq);
    }
}

static void update_notification_handler(void *context, void *data) {
    uint32_t freq = rc_parse_frequency((const char *)data);
    if (freq != 0) {
        ESP_LOGI(TAG, "Received frequency update: %lu", freq);
        if (current_frequency != freq) {
            send_frequency_change_event(freq);
        }
    }
}

void init_op_mode() {
    default_op_mode = (op_mode_t *)calloc(1, sizeof(op_mode_t));
    if (default_op_mode == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for default op default_op_mode");
        return;
    }

    default_op_mode->id = 0;
    strncpy(default_op_mode->name, "Default", MAX_OP_MODE_NAME_LEN);
    default_op_mode->order = MAX_ORDER;
    default_op_mode->freq_ranges = linked_list_create();
    freq_range_t *default_range = (freq_range_t *)calloc(1, sizeof(freq_range_t));
    default_range->start = 0;
    default_range->end = 47000000;
    default_op_mode->commands = strdup("");
    linked_list_push(default_op_mode->freq_ranges, default_range);

    op_modes = linked_list_create();
    if (op_modes == NULL) {
        ESP_LOGE(TAG, "Failed to create linked list for op modes");
        return;
    }
    commands_list = linked_list_create();
    if (commands_list == NULL) {
        ESP_LOGE(TAG, "Failed to create linked list for op mode commands");
        return;
    }

    om_event_queue = xQueueCreate(10, sizeof(om_event_t *));
    if (om_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create op mode event queue");
        return;
    }

    // create a task to process the op mode events
    if (xTaskCreate(op_mode_task, "op_mode_task", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create rig monitor task");
        return;
    }

    send_event(OM_LOAD);

    rig_monitor_add_observers(OBSERVE_STATUS, status_notification_handler, NULL);
    rig_monitor_add_observers(OBSERVE_COMMANDS, command_notification_handler, NULL);
    rig_monitor_add_observers(OBSERVE_UPDATES, update_notification_handler, NULL);

    ESP_LOGI(TAG, "Op mode initialized");
}
