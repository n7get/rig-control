#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_littlefs.h"
#include "esp_log.h"
#include "string.h"
#include "config.h"
#include "http.h"
#include "network.h"
#include "settings.h"
#include "uart.h"
#include "esp_timer.h"

#define TAG "MAIN"

static void mount_html(void) {
    esp_vfs_littlefs_conf_t conf = {
        .base_path = HTML_MOUNT_POINT,
        .partition_label = "html",
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find HTML partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get HTML partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "HTML partition size: total: %d, used: %d", total, used);
    }
}

static int64_t last_data_time = 0;

static void recv_data_callback(void *context, void *data) {
    recv_result_t *result = (recv_result_t *)data;

    int64_t current_time = esp_timer_get_time();
    int64_t elapsed_time_ms = (current_time - last_data_time) / 1000;

    if (result->error != ESP_OK) {
        ESP_LOGE(TAG, "Error receiving data: %s", result->data);
        return;
    }

    ESP_LOGI(TAG, "Received data: %s (elapsed time: %lld ms)", result->data, elapsed_time_ms);

    last_data_time = esp_timer_get_time();
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting application");
    ESP_LOGI(TAG, "FreeRTOS version: %s", tskKERNEL_VERSION_NUMBER);

    mount_html();

    load_settings();
    ESP_LOGI(TAG, "Loaded settings: AP SSID=%s, STA SSID=%s", ap_ssid, sta_ssid);

    wifi_init();

    if (!start_webserver()) {
        ESP_LOGE(TAG, "Failed to start web server");
        return;
    }

    register_settings_endpoints();

    uart_init();
    uart_add_recv_observer(recv_data_callback, NULL);

    char *cmds[] = {"BY;", "FA;", "AI;"};

    while (1) {
        for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
            esp_err_t err = uart_send(cmds[i], SEND_TYPE_EXTERNAL);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to send command");
                vTaskDelete(NULL);
                return;
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
