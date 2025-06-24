#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "cJSON.h"

#include "config.h"
#include "group.h"
#include "linked_list.h"
#include "ui.h"

static linked_list_t *groups;
static SemaphoreHandle_t groups_mutex = NULL;

#define TAG "GROUP"

static void safe_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    } else {
        ESP_LOGE(TAG, "Attempted to free a NULL pointer");
    }
}

static void free_group(group_t *group) {
    if (group == NULL) {
        return;
    }
    if (group->modes != NULL) {
        safe_free(group->modes);
    }
    if (group->properties != NULL) {
        safe_free(group->properties);
    }
    safe_free(group);
}

#define MAX_LOG_BUFFER_SIZE 256
void log_group(const char *tag, const char *prefix, const group_t *group) {
    char *buf = (char *)malloc(MAX_LOG_BUFFER_SIZE);

    int len = 0;
    if (prefix != NULL) {
        strcpy(buf, prefix);
        len = strlen(prefix);
    }

    len += sprintf(buf + len, "id: %d", group->id);
    len += sprintf(buf + len, ", Name: %s", group->name);
    len += sprintf(buf + len, ", Mode: %s", group->modes);
    len += sprintf(buf + len, ", Order: %d", group->order);
    len += sprintf(buf + len, ", Parameters: %s", group->properties);

    ESP_LOGI(tag ? tag : TAG, "%s", buf);

    safe_free(buf);
}

static void send_result(const char *result, const char *message) {
    if (result == NULL || message == NULL) {
        ESP_LOGE(TAG, "Invalid result or message");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "group");
    cJSON_AddStringToObject(json, "event", result);
    cJSON_AddStringToObject(json, "value", message);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str == NULL) {
        ESP_LOGE(TAG, "Failed to print group JSON");
        cJSON_Delete(json);
        return;
    }

    ESP_LOGI(TAG, "Result: %s", json_str);
    ui_send_json(json_str);
    
    safe_free(json_str);
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

static int get_next_id() {
    int next_id;
    if (get_u32("next_group_id", (uint32_t *)&next_id) != ESP_OK) {
        next_id = 4;
    }

    set_u32("next_group_id", (uint32_t)(next_id + 1));

    return next_id;
}

static void make_key(char *key, int id) {
    snprintf(key, 11, "%d", id);
}

static char *encode_group(const group_t *group) {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    cJSON_AddNumberToObject(root, "id", group->id);
    cJSON_AddStringToObject(root, "name", group->name);
    cJSON_AddStringToObject(root, "modes", group->modes);
    cJSON_AddNumberToObject(root, "order", group->order);
    cJSON_AddStringToObject(root, "properties", group->properties);

    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        ESP_LOGE(TAG, "Failed to print group JSON");
        cJSON_Delete(root);
        return NULL;
    }

    cJSON_Delete(root);
    return json_str;
}

static esp_err_t process_decode_group(cJSON *root, group_t *group) {
    cJSON *id = cJSON_GetObjectItem(root, "id");
    if (id == NULL || !cJSON_IsNumber(id)) {
        ESP_LOGE(TAG, "Invalid or missing 'id' in group JSON");
        return ESP_FAIL;
    }
    group->id = id->valueint;

    cJSON *name = cJSON_GetObjectItem(root, "name");
    if (name == NULL || !cJSON_IsString(name)) {
        ESP_LOGE(TAG, "Invalid or missing 'name' in group JSON");
        return ESP_FAIL;
    }
    strncpy(group->name, name->valuestring, MAX_GROUP_NAME_LEN);

    cJSON *modes = cJSON_GetObjectItem(root, "modes");
    if (modes == NULL || !cJSON_IsString(modes)) {
        ESP_LOGE(TAG, "Invalid or missing 'modes' in group JSON");
        return ESP_FAIL;
    }
    group->modes = strdup(modes->valuestring);

    cJSON *order = cJSON_GetObjectItem(root, "order");
    if (order == NULL || !cJSON_IsNumber(order)) {
        ESP_LOGE(TAG, "Invalid or missing 'order' in group JSON");
        return ESP_FAIL;
    }
    group->order = order->valueint;

    cJSON *properties = cJSON_GetObjectItem(root, "properties");
    if (properties == NULL || !cJSON_IsString(properties)) {
        ESP_LOGE(TAG, "Invalid or missing 'properties' in group JSON");
        return ESP_FAIL;
    }
    group->properties = strdup(properties->valuestring);

    return ESP_OK;
}

static group_t *decode_group(const uint8_t *json, size_t json_size) {
    if (json == NULL || json_size == 0) {
        ESP_LOGE(TAG, "decode_group: input json is NULL or size is 0");
        return NULL;
    }
    
    cJSON *root = cJSON_ParseWithLength((const char *)json, json_size);
    if (root == NULL) {
        ESP_LOGE(TAG, "decode_group: failed to parse JSON");
        return NULL;
    }

    group_t *group = (group_t *)calloc(1, sizeof(group_t));
    if (group == NULL) {
        ESP_LOGE(TAG, "decode_group: failed to allocate group");
        cJSON_Delete(root);
        return NULL;
    }

    if (process_decode_group(root, group) != ESP_OK) {
        ESP_LOGE(TAG, "decode_group: failed to decode group from JSON");
        free_group(group);
        cJSON_Delete(root);
        return NULL;
    }

    cJSON_Delete(root);

    return group;
}

static bool group_exists(const char *name) {
    for (linked_list_node_t *node = linked_list_begin(groups); node != NULL; node = linked_list_next(node)) {
        group_t *group = (group_t *)node->data;
        if (strcmp(group->name, name) == 0) {
            return true;
        }
    }
    return false;
}

static group_t *get_group(int id) {
    if (groups == NULL) {
        ESP_LOGE(TAG, "get_group: groups list is NULL");
        return NULL;
    }
    for (linked_list_node_t *node = linked_list_begin(groups); node != NULL; node = linked_list_next(node)) {
        group_t *group = (group_t *)node->data;
        if (group != NULL && group->id == id) {
            return group;
        }
    }
    ESP_LOGE(TAG, "get_group: group with id %d not found", id);
    return NULL;
}

static esp_err_t load_group(nvs_handle_t nvs_handle, const char *key) {
    size_t required_size = 0;
    esp_err_t err = nvs_get_str(nvs_handle, key, NULL, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read group size from NVS: %s", esp_err_to_name(err));
        return ESP_FAIL;
    }

    char *buffer = malloc(required_size);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for group buffer: %d", (int)required_size);
        return ESP_FAIL;
    }

    err = nvs_get_str(nvs_handle, key, buffer, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read group from NVS: %s", esp_err_to_name(err));
        safe_free(buffer);
        return ESP_FAIL;
    }

    group_t *group = decode_group((const uint8_t *)buffer, required_size);
    safe_free(buffer);
    if (group == NULL) {
        ESP_LOGE(TAG, "Failed to decode group from JSON");
        return ESP_FAIL;
    }

    log_group(TAG, "Loaded group: ", group);
    linked_list_push(groups, group);

    return ESP_OK;
}

static void handle_load() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(GROUP_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "handle_load: failed to open NVS: %s", esp_err_to_name(err));
        return;
    }

    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find("nvs", GROUP_NAMESPACE, NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        load_group(nvs_handle, info.key);
        res = nvs_entry_next(&it);
    }

    nvs_release_iterator(it);
    nvs_close(nvs_handle);
}

static esp_err_t save_to_nvs(group_t *group) {
    if (group == NULL) {
        ESP_LOGE(TAG, "save_to_nvs: group is NULL");
        return ESP_FAIL;
    }

    if (group->id == 0) {
        group->id = get_next_id();
    }

    char *g_str = encode_group(group);
    if (g_str == NULL) {
        ESP_LOGE(TAG, "save_to_nvs: failed to encode group");
        return ESP_FAIL;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(GROUP_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "save_to_nvs: failed to open NVS: %s", esp_err_to_name(err));
        safe_free(g_str);
        return err;
    }

    char key[12];
    make_key(key, group->id);
    err = nvs_set_str(nvs_handle, key, g_str);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "save_to_nvs: failed to write group to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        safe_free(g_str);
        return err;
    }

    if (nvs_commit(nvs_handle) != ESP_OK) {
        ESP_LOGE(TAG, "save_to_nvs: failed to commit group to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        safe_free(g_str);
        return err;
    }

    nvs_close(nvs_handle);
    safe_free(g_str);
    ESP_LOGI(TAG, "Group %d persisted successfully", group->id);
    return ESP_OK;
}

static void remove_from_nvs(group_t *group) {
    if (group == NULL || group->id == 0) {
        ESP_LOGE(TAG, "remove_from_nvs: group is NULL or id is 0");
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(GROUP_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        error_log("remove_from_nvs(): failed to open NVS for groups: %s", esp_err_to_name(err));
        return;
    }

    char key[12];
    make_key(key, group->id);

    nvs_erase_key(nvs_handle, key);

    nvs_close(nvs_handle);
}

static void send_group(const group_t *group) {
    if (group == NULL) {
        ESP_LOGE(TAG, "send_group: group is NULL");
        return;
    }

    char *g_str = encode_group(group);
    if (g_str == NULL) {
        ESP_LOGE(TAG, "send_group: failed to encode group");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        ESP_LOGE(TAG, "send_group: failed to create JSON object");
        safe_free(g_str);
        return;
    }

    cJSON_AddStringToObject(json, "topic", "group");
    cJSON_AddStringToObject(json, "event", "update");
    cJSON_AddRawToObject(json, "value", g_str);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str != NULL) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "send_group: failed to print group JSON");
    }

    safe_free(g_str);
    cJSON_Delete(json);
}

static void send_deleted(const group_t *group) {
    if (group == NULL) {
        ESP_LOGE(TAG, "send_deleted: group is NULL");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        ESP_LOGE(TAG, "send_deleted: failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "group");
    cJSON_AddStringToObject(json, "event", "removed");
    cJSON_AddNumberToObject(json, "value", group->id);
    char *json_str = cJSON_PrintUnformatted(json);

    if (json_str != NULL) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "send_deleted: failed to print group JSON");
    }

    cJSON_Delete(json);
}

static void create_group(group_t *group) {
    if (group_exists(group->name)) {
        error_log("group already exists: %s", group->name);
        free_group(group);
        return;
    }

    if (group->id != 0) {
        error_log("group ID should be 0 for new groups, got: %d", group->id);
        free_group(group);
        return;
    }

    if (save_to_nvs(group) != ESP_OK) {
        error_log("Failed to persist group");
        free_group(group);
        return;
    }

    linked_list_push(groups, group);

    send_group(group);
}

static void update_group(group_t *group) {
    if (group->id == 0) {
        error_log("group ID 0 cannot be updated");
        return;
    }

    group_t *old_group = get_group(group->id);
    if (!old_group) {
        error_log("group not found: %d", group->id);
        return;
    }

    if (save_to_nvs(group) != ESP_OK) {
        error_log("Failed to persist group");
        free_group(group);
        return;
    }

    linked_list_remove(groups, old_group);
    free_group(old_group);

    linked_list_push(groups, group);
    send_group(group);
}

static void send_refresh() {
    ui_send_json("{\"topic\":\"group\",\"event\":\"refresh\",\"value\":\"start\"}");
    for (linked_list_node_t *node = linked_list_begin(groups); node != NULL; node = linked_list_next(node)) {
        send_group((group_t *)node->data);
    }
    ui_send_json("{\"topic\":\"group\",\"event\":\"refresh\",\"value\":\"complete\"}");
}

void send_groups_config() {
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }
    cJSON_AddStringToObject(json, "topic", "group");
    cJSON_AddStringToObject(json, "event", "config");
    cJSON *config = cJSON_CreateObject();
    if (!config) {
        cJSON_Delete(json);
        return;
    }
    cJSON_AddNumberToObject(config, "MAX_GROUP_NAME_LEN", MAX_GROUP_NAME_LEN);
    cJSON_AddItemToObject(json, "value", config);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        ESP_LOGE(TAG, "Failed to print JSON");
    }
    cJSON_Delete(json);
}

static void remove_group(int id) {
    if (id == 0) {
        error_log("group ID 0 cannot be removed");
        return;
    }

    group_t *group = get_group(id);
    if (!group) {
        error_log("group not found: %d", id);
        return;
    }

    remove_from_nvs(group);

    send_deleted(group);

    linked_list_remove(groups, group);
    free_group(group);
}

static void handle_reset() {
    while (linked_list_size(groups) > 0) {
        linked_list_node_t *node = linked_list_begin(groups);
        group_t *chan = (group_t *)node->data;
        remove_group(chan->id);
    }
}

static void gr_recv_from_ui_in_mutex(cJSON *json_obj) {
    cJSON *eventValue = cJSON_GetObjectItem(json_obj, "event");
    if (eventValue == NULL || !cJSON_IsString(eventValue)) {
        error_log("Missing or invalid event");
        return;
    }

    char *event = eventValue->valuestring;
    if (event == NULL || strlen(event) == 0) {
        error_log("Missing or invalid event string");
        return;
    }

    if (strcmp(event, "refresh") == 0) {
        send_refresh();
        send_groups_config();
        return;
    }

    if(strcmp(event, "reset") == 0) {
        handle_reset();
        return;
    }

    cJSON *value_obj = cJSON_GetObjectItem(json_obj, "value");
    if (value_obj == NULL || !cJSON_IsObject(value_obj)) {
        error_log("Missing or invalid value parameter");
        return;
    }

    cJSON *idValue = cJSON_GetObjectItem(value_obj, "id");
    int id = idValue && cJSON_IsNumber(idValue) ? idValue->valueint : 0;

    if (strcmp(event, "remove") == 0) {
        remove_group(id);
        return;
    }

    group_t *group = (group_t *)calloc(1, sizeof(group_t));
    if (group == NULL) {
        error_log("Failed to allocate memory for group");
        return;
    }

    if (process_decode_group(value_obj, group) != ESP_OK) {
        free_group(group);
        return;
    }

    if (strcmp(event, "create") == 0) {
        create_group(group);
        return;
    }

    if (strcmp(event, "update") == 0) {
        update_group(group);
        return;
    }

    free_group(group);
    error_log("Unknown group event: %s", event);
}

void gr_recv_from_ui(cJSON *json_obj) {
    if (xSemaphoreTake(groups_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take groups mutex");
        return;
    }

    gr_recv_from_ui_in_mutex(json_obj);

    xSemaphoreGive(groups_mutex);
}

void group_init() {
    groups_mutex = xSemaphoreCreateMutex();

    groups = linked_list_create();
    if (!groups) {
        ESP_LOGE(TAG, "Failed to create linked list for groups");
        return;
    }

    handle_load();

    ESP_LOGI(TAG, "group initialized");
}
