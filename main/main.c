#include "esp_littlefs.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "config.h"
#include "group.h"
#include "http.h"
#include "info.h"
#include "mem_chan.h"
#include "op_mode.h"
#include "rig.h"
#include "rig_monitor.h"
#include "rig_commands.h"
#include "rig_controller.h"
#include "settings.h"
#include "ui.h"
#include "wifi.h"
#include "web_socket.h"

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

void app_main(void) {
    ESP_LOGI(TAG, "Starting application");
    ESP_LOGI(TAG, "FreeRTOS version: %s", tskKERNEL_VERSION_NUMBER);
    // ESP_LOGI(TAG, "Free heap size: %u", esp_get_free_heap_size());
    // ESP_LOGI(TAG, "Free stack size: %u", uxTaskGetStackHighWaterMark(NULL));
    // ESP_LOGI(TAG, "Minimum free heap size: %u", esp_get_minimum_free_heap_size());

    ESP_ERROR_CHECK(esp_timer_early_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(nvs_flash_init());

    mount_html();

    load_settings();
    ESP_LOGI(TAG, "Loaded settings: AP SSID=%s, STA SSID=%s", ap_ssid, sta_ssid);

    wifi_init();
    if (!start_webserver()) {
        ESP_LOGE(TAG, "Failed to start web server");
        return;
    }

    ws_server_start();
    
    init_info();
    cat_init();    
    init_rig_commands();
    init_rig_monitor();
    ui_init();
    init_rig_controller();
    init_op_mode();
    init_mem_chan();
    group_init();
    
    ESP_LOGI(TAG, "Application started");
}
