#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "config.h"
#include "linked_list.h"
#include "mem_chan.h"
#include "ui.h"

static linked_list_t *mem_chans;
static SemaphoreHandle_t mem_chans_mutex = NULL;

#define TAG "MEM_CHAN"

static void safe_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    } else {
        ESP_LOGE(TAG, "Attempted to free a NULL pointer");
    }
}

void mc_free_mem_chan(mem_chan_t *chan) {
    if (chan == NULL) {
        return;
    }

    if (chan->commands != NULL) {
        safe_free(chan->commands);
    }
    safe_free(chan);
}

#define MAX_LOG_BUFFER_SIZE 256
void log_mem_chan(char *tag, char *prefix, const mem_chan_t *chan) {
    char *buf = (char *)malloc(MAX_LOG_BUFFER_SIZE);
    int len = 0;

    if (prefix != NULL) {
        strcpy(buf, prefix);
        len = strlen(prefix);
    }

    len += sprintf(buf + len, "id: %lu", chan->id);
    len += sprintf(buf + len, ", Name: %s", chan->name);
    len += sprintf(buf + len, ", Frequency: %lu", chan->frequency);
    len += sprintf(buf + len, ", Op Mode: %s", chan->op_mode);
    len += sprintf(buf + len, ", Commands: %s", chan->commands);

    ESP_LOGI(TAG, "%s", buf);
    safe_free(buf);
}

static void free_mem_chan(mem_chan_t *chan) {
    if (chan == NULL) {
        return;
    }
    if (chan->commands != NULL) {
        safe_free(chan->commands);
    }
    safe_free(chan);
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
    cJSON_AddStringToObject(json, "topic", "mem_chan");
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
    if (get_u32("next_memchan_id", &next_id) != ESP_OK) {
        next_id = 1;
    }
    set_u32("next_memchan_id", next_id + 1);
    return next_id;
}

static void make_key(char *key, uint32_t id) {
    if (key == NULL) {
        ESP_LOGE(TAG, "Key buffer is NULL");
        return;
    }
    snprintf(key, 11, "%lu", id);
}

static char *encode_mem_chan(const mem_chan_t *chan) {
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    cJSON_AddNumberToObject(root, "id", chan->id);
    cJSON_AddStringToObject(root, "name", chan->name);
    cJSON_AddNumberToObject(root, "frequency", chan->frequency);
    cJSON_AddStringToObject(root, "op_mode", chan->op_mode);
    cJSON_AddStringToObject(root, "commands", chan->commands);

    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        ESP_LOGE(TAG, "Failed to print mem_chan JSON");
        cJSON_Delete(root);
        return NULL;
    }

    cJSON_Delete(root);
    return json_str;
}

static esp_err_t process_decode_mem_chan(cJSON *root, mem_chan_t *chan) {
    cJSON *id = cJSON_GetObjectItem(root, "id");
    if (id == NULL || !cJSON_IsNumber(id)) {
        ESP_LOGE(TAG, "Invalid or missing 'id' in mem_chan JSON");
        return ESP_FAIL;
    }
    chan->id = (uint32_t)id->valueint;

    cJSON *name = cJSON_GetObjectItem(root, "name");
    if (name == NULL || !cJSON_IsString(name)) {
        ESP_LOGE(TAG, "Invalid or missing 'name' in mem_chan JSON");
        return ESP_FAIL;
    }
    strncpy(chan->name, name->valuestring, MAX_MEM_CHAN_NAME_LEN);

    cJSON *frequency = cJSON_GetObjectItem(root, "frequency");
    if (frequency == NULL || !cJSON_IsNumber(frequency)) {
        ESP_LOGE(TAG, "Invalid or missing 'frequency' in mem_chan JSON");
        return ESP_FAIL;
    }
    chan->frequency = (uint32_t)frequency->valuedouble;

    cJSON *op_mode = cJSON_GetObjectItem(root, "op_mode");
    if (op_mode == NULL || !cJSON_IsString(op_mode)) {
        ESP_LOGE(TAG, "Invalid or missing 'op_mode' in mem_chan JSON");
        return ESP_FAIL;
    }
    strncpy(chan->op_mode, op_mode->valuestring, MAX_OP_MODE_NAME_LEN);

    cJSON *commands = cJSON_GetObjectItem(root, "commands");
    if (commands == NULL || !cJSON_IsString(commands)) {
        ESP_LOGE(TAG, "Invalid or missing 'commands' in mem_chan JSON");
        return ESP_FAIL;
    }
    chan->commands = strdup(commands->valuestring);

    return ESP_OK;
}

static mem_chan_t *decode_mem_chan(const uint8_t *json, size_t json_size) {
    if (json == NULL || json_size == 0) {
        return NULL;
    }
    cJSON *root = cJSON_ParseWithLength((const char *)json, json_size);
    if (!root) {
        return NULL;
    }
    mem_chan_t *chan = (mem_chan_t *)calloc(1, sizeof(mem_chan_t));
    if (!chan) {
        cJSON_Delete(root);
        return NULL;
    }
    if (process_decode_mem_chan(root, chan) != ESP_OK) {
        free_mem_chan(chan);
        cJSON_Delete(root);
        return NULL;
    }
    cJSON_Delete(root);
    return chan;
}

static bool mem_chan_exists(const char *name) {
    if (name == NULL) {
        ESP_LOGE(TAG, "mem_chan name is NULL");
        return false;
    }
    for (linked_list_node_t *node = linked_list_begin(mem_chans); node != NULL; node = linked_list_next(node)) {
        mem_chan_t *chan = (mem_chan_t *)node->data;
        if (strcmp(chan->name, name) == 0) {
            return true;
        }
    }
    return false;
}

static mem_chan_t *get_mem_chan(const uint32_t id) {
    for (linked_list_node_t *node = linked_list_begin(mem_chans); node != NULL; node = linked_list_next(node)) {
        mem_chan_t *chan = (mem_chan_t *)node->data;
        if (chan != NULL && chan->id == id) {
            return chan;
        }
    }
    return NULL;
}

static esp_err_t load_mem_chan(nvs_handle_t nvs_handle, const char *key) {
    size_t required_size = 0;
    esp_err_t err = nvs_get_str(nvs_handle, key, NULL, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read mem_chan size from NVS: %s", esp_err_to_name(err));
        return ESP_FAIL;
    }

    char *buffer = malloc(required_size);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for mem_chan buffer: %d", (int)required_size);
        return ESP_FAIL;
    }

    err = nvs_get_str(nvs_handle, key, buffer, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read mem_chan from NVS: %s", esp_err_to_name(err));
        safe_free(buffer);
        return ESP_FAIL;
    }

    mem_chan_t *chan = decode_mem_chan((const uint8_t *)buffer, required_size);
    safe_free(buffer);
    if (chan == NULL) {
        ESP_LOGE(TAG, "Failed to decode mem_chan from JSON");
        return ESP_FAIL;
    }
log_mem_chan(TAG, "Loaded mem_chan: ", chan);
    linked_list_push(mem_chans, chan);
    return ESP_OK;
}

static void handle_load() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(MEM_CHaN_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "handle_load(): failed to open NVS for mem_chans: %s", esp_err_to_name(err));
        return;
    }
    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find("nvs", MEM_CHaN_NAMESPACE, NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        if (load_mem_chan(nvs_handle, info.key) != ESP_OK) {
            nvs_release_iterator(it);
            nvs_close(nvs_handle);
            return;
        }
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);
}

static esp_err_t save_to_nvs(mem_chan_t *chan) {
    if (chan == NULL) {
        return ESP_FAIL;
    }
log_mem_chan(TAG, "Saving mem_chan: ", chan);
    if (chan->id == 0) {
        chan->id = get_next_id();
    }

    ESP_LOGI(TAG, "Persisting mem_chan %lu", chan->id);

    char *mc_str = encode_mem_chan(chan);
    if (mc_str == NULL) {
        ESP_LOGE(TAG, "Failed to encode mem_chan");
        return ESP_FAIL;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(MEM_CHaN_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS for mem_chans: %s", esp_err_to_name(err));
        safe_free(mc_str);
        return err;
    }

    char key[11];
    make_key(key, chan->id);

    ESP_LOGI(TAG, "Writing mem_chan '%s' to NVS with key '%s'", chan->name, key);
    log_mem_chan(TAG, "Persisting mem_chan: ", chan);

    err = nvs_set_str(nvs_handle, key, mc_str);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write mem_chan to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        safe_free(mc_str);
        return err;
    }

    if (nvs_commit(nvs_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit mem_chan to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        safe_free(mc_str);
        return err;
    }

    nvs_close(nvs_handle);
    safe_free(mc_str);

    ESP_LOGI(TAG, "mem_chan '%lu' persisted successfully", chan->id);
    return ESP_OK;
}

static void remove_from_nvs(mem_chan_t *chan) {
    if (chan == NULL || chan->id == 0) {
        return;
    }
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(MEM_CHaN_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        error_log("remove_from_nvs(): failed to open NVS for mem_chans: %s", esp_err_to_name(err));
        return;
    }
    char key[11];
    make_key(key, chan->id);
    ESP_LOGI(TAG, "Removing mem_chan '%s' from NVS with key '%s'", chan->name, key);
    err = nvs_erase_key(nvs_handle, key);
    if (err != ESP_OK) {
        error_log("Failed to erase mem_chan from NVS: %s", esp_err_to_name(err));
        return;
    }
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "mem_chan '%lu' removed successfully", chan->id);
}

static void send_mem_chan(const mem_chan_t *chan) {
    if (chan == NULL) {
        error_log("mem_chan is NULL");
        return;
    }
log_mem_chan(TAG, "Sending mem_chan: ", chan);
    char *mc_str = encode_mem_chan(chan);
    if (mc_str == NULL) {
        error_log("Failed to encode mem_chan");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        error_log("Failed to create JSON object");
        safe_free(mc_str);
        return;
    }

    cJSON_AddStringToObject(json, "topic", "mem_chan");
    cJSON_AddStringToObject(json, "event", "update");
    cJSON_AddRawToObject(json, "value", mc_str);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str != NULL) {
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        error_log("Failed to print mem_chan JSON");
    }
    safe_free(mc_str);
    cJSON_Delete(json);
}

static void send_deleted(const mem_chan_t *chan) {
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        error_log("Failed to create JSON object");
        return;
    }

    cJSON_AddStringToObject(json, "topic", "mem_chan");
    cJSON_AddStringToObject(json, "event", "removed");
    cJSON_AddNumberToObject(json, "value", chan->id);

    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str != NULL) {
        ESP_LOGI(TAG, "Sending mem_chan: %s", json_str);
        ui_send_json(json_str);
        safe_free(json_str);
    } else {
        error_log("Failed to print mem_chan JSON");
    }

    cJSON_Delete(json);
}

static void create_mem_chan(mem_chan_t *chan) {
    if (mem_chan_exists(chan->name)) {
        error_log("mem_chan already exists: %s", chan->name);
        free_mem_chan(chan);
        return;
    }

    if (chan->id != 0) {
        error_log("mem_chan ID should be 0 for new chans, got: %lu", chan->id);
        free_mem_chan(chan);
        return;
    }

    if (save_to_nvs(chan) != ESP_OK) {
        error_log("Failed to persist mem_chan");
        free_mem_chan(chan);
        return;
    }
log_mem_chan(TAG, "Creating mem_chan: ", chan);
    linked_list_push(mem_chans, chan);
    log_mem_chan(TAG, "Created mem_chan: ", chan);

    send_mem_chan(chan);
}

void send_mem_chan_config() {
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return;
    }
    cJSON_AddStringToObject(json, "topic", "mem_chan");
    cJSON_AddStringToObject(json, "event", "config");
    cJSON *config = cJSON_CreateObject();
    if (!config) {
        cJSON_Delete(json);
        return;
    }
    cJSON_AddNumberToObject(config, "MAX_MEM_CHAN_NAME_LEN", MAX_MEM_CHAN_NAME_LEN);
    cJSON_AddNumberToObject(config, "MAX_ORDER", MAX_ORDER);
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

static void send_refresh() {
    ui_send_json("{\"topic\":\"mem_chan\",\"event\":\"refresh\",\"value\":\"start\"}");
    for (linked_list_node_t *node = linked_list_begin(mem_chans); node != NULL; node = linked_list_next(node)) {
        send_mem_chan((mem_chan_t *)node->data);
    }
    ui_send_json("{\"topic\":\"mem_chan\",\"event\":\"refresh\",\"value\":\"complete\"}");
}

static void remove_mem_chan(uint32_t id) {
    if (id == 0) {
        error_log("mem_chan ID 0 cannot be removed");
        return;
    }
    mem_chan_t *chan = get_mem_chan(id);
    if (chan == NULL) {
        error_log("mem_chan not found: %lu", id);
        return;
    }
    log_mem_chan(TAG, "Removing mem_chan: ", chan);
    remove_from_nvs(chan);
    send_deleted(chan);
    linked_list_remove(mem_chans, chan);
    free_mem_chan(chan);
}

static void update_mem_chan(mem_chan_t *chan) {
    if (chan == NULL) {
        error_log("mem_chan is NULL");
        return;
    }
log_mem_chan(TAG, "Updating mem_chan: ", chan);
    if (chan->id == 0) {
        error_log("mem_chan ID 0 cannot be updated");
        return;
    }

    mem_chan_t *old_chan = get_mem_chan(chan->id);
    if (old_chan == NULL) {
        error_log("mem_chan not found: %lu", chan->id);
        return;
    }

    if (save_to_nvs(chan) != ESP_OK) {
        error_log("Failed to persist mem_chan");
        free_mem_chan(chan);
        return;
    }

    linked_list_remove(mem_chans, old_chan);
    free_mem_chan(old_chan);
    linked_list_push(mem_chans, chan);

    log_mem_chan(TAG, "Updated mem_chan: ", chan);
    send_mem_chan(chan);
}

static void mc_recv_from_ui_in_mutex(cJSON *json_obj) {
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

    if (strcmp(event, "refresh") == 0) {
        send_refresh();
        send_mem_chan_config();
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
        remove_mem_chan(id);
        return;
    }

    mem_chan_t *chan = (mem_chan_t *)calloc(1, sizeof(mem_chan_t));
    if (!chan) {
        error_log("Failed to allocate memory for mem_chan");
        return;
    }

    if (process_decode_mem_chan(value_obj, chan) != ESP_OK) {
        free_mem_chan(chan);
        return;
    }

    if (strcmp(event, "create") == 0) {
        create_mem_chan(chan);
        return;
    }

    if (strcmp(event, "update") == 0) {
        update_mem_chan(chan);
        return;
    }

    safe_free(chan);
    error_log("Unknown mem_chan event: %s", event);
}

void mc_recv_from_ui(cJSON *json_obj) {
    if (xSemaphoreTake(mem_chans_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        error_log("Failed to take mem_chans mutex");
        return;
    }

    mc_recv_from_ui_in_mutex(json_obj);

    xSemaphoreGive(mem_chans_mutex);
}

static mem_chan_t *mc_find_by_name_in_mutex(const char *name) {
    for (linked_list_node_t *node = linked_list_begin(mem_chans); node != NULL; node = linked_list_next(node)) {
        mem_chan_t *chan = (mem_chan_t *)node->data;
        if (strcmp(chan->name, name) == 0) {
            return chan;
        }
    }
    return NULL;
}

mem_chan_t *mc_find_by_name(const char *name) {
    if (name == NULL || strlen(name) == 0) {
        error_log("Invalid mem_chan name");
        return NULL;
    }

    if (xSemaphoreTake(mem_chans_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        error_log("Failed to take mem_chans mutex");
        return NULL;
    }

    mem_chan_t *chan = mc_find_by_name_in_mutex(name);

    if (chan == NULL ) {
        xSemaphoreGive(mem_chans_mutex);
        return NULL;
    }

    mem_chan_t *result = calloc(1, sizeof(mem_chan_t));
    if (result == NULL) {
        ESP_LOGE(TAG, "mem_chan not found: %s", name);
        xSemaphoreGive(mem_chans_mutex);
        return NULL;
    }

    result->id = chan->id;
    strncpy(result->name, chan->name, MAX_MEM_CHAN_NAME_LEN);
    result->frequency = chan->frequency;
    strncpy(result->op_mode, chan->op_mode, MAX_OP_MODE_NAME_LEN);
    result->commands = strdup(chan->commands);
    if (result->commands == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for mem_chan commands");
        safe_free(result);
        xSemaphoreGive(mem_chans_mutex);
        return NULL;
    }
    log_mem_chan(TAG, "Found mem_chan: ", result);

    xSemaphoreGive(mem_chans_mutex);

    return result;
}

void init_mem_chan() {
    mem_chans_mutex = xSemaphoreCreateMutex();

    mem_chans = linked_list_create();
    if (mem_chans == NULL) {
        ESP_LOGE(TAG, "Failed to create linked list for mem_chans");
        return;
    }

    handle_load();
    ESP_LOGI(TAG, "mem_chan initialized");
}
