#include "esp_littlefs.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "cat.h"
#include "config.h"
#include "http.h"
#include "info.h"
#include "network.h"
#include "rig_monitor.h"
#include "rig_commands.h"
#include "settings.h"

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

// static int64_t last_data_time = 0;

// static void recv_data_callback(void *context, void *data) {
//     ESP_LOGI(TAG, "Received data: %s", (char *)data);
// }

void app_main(void) {
    esp_timer_early_init();

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

    register_info_endpoints();
    register_settings_endpoints();

    cat_init();
    
    init_rig_commands();
    init_rig_monitor();
    
    ESP_LOGI(TAG, "Application started");
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    // rig_monitor_add_observers(OBSERVE_UPDATES | OBSERVE_STATUS, recv_data_callback, NULL);
    // rig_monitor_add_observers(OBSERVE_COMMANDS, recv_data_callback, NULL);
    // rig_monitor_add_observers(OBSERVE_STATUS, recv_data_callback, NULL);
    // char *cmds[] = {"BY;", "FA;"};

    // while (1) {
    //     for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
    //         esp_err_t err = rig_monitor_send(cmds[i], SEND_TYPE_COMMAND);
    //         if (err != ESP_OK) {
    //             ESP_LOGE(TAG, "Failed to send command");
    //             vTaskDelete(NULL);
    //             return;
    //         }
    //     }

    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}
