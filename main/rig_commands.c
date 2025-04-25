#include <stdio.h>
#include <string.h>
// #include <stdlib.h>
// #include <time.h>
// #include <stdbool.h>
#include "rig_commands.h"
#include "uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TAG "RIG_COMMANDS"

// Updated rig_command_t initialization to include 'len' field, calculated as the length of cmd - 1
rig_command_t rig_commands[] = {
    {"AC;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"AG;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"BC;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"BI;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"BP00;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"BP01;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"BY;", 2, false, 100, 0, NULL},
    {"CN00;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"CN01;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"CO00;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"CO01;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"CO02;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"CO03;", 4, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"CS;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"CT;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"DA;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"FA;", 2, false, 100, 0, NULL},
    {"FB;", 2, false, 100, 0, NULL},
    {"FS;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"FT;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"GT;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"ID;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"IF;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"IS;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"KM;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"KP;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"KR;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"KS;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"LK;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"LM;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"MD;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"MG;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"ML0;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"ML1;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"MS;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"MX;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"NA;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"NB;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"NL;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"NR;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"OI;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"OS;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"PA;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"PC;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"PL;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"PR0;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"PR1;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RA;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RG;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RI0;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RI3;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RI4;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RI5;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RI6;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RI7;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RIA;", 3, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RL;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"RT;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"SD;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"SH;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"SQ;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"TS;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"TX;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"UL;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"VD;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"VG;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"VX;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"XT;", 2, false, DEFAULT_REFRESH_TIME, 0, NULL},
    {"EX001;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX002;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX003;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX004;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX005;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX006;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX007;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX008;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX009;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX010;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX011;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX012;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX013;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX014;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX015;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX016;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX017;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX018;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX019;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX020;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX021;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX022;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX023;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX024;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX025;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX026;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX027;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX028;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX029;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX030;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX031;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX032;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX033;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX034;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX035;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX036;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX037;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX038;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX039;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX040;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX041;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX042;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX043;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX044;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX045;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX046;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX047;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX048;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX049;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX050;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX051;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX052;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX053;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX054;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX055;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX056;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX057;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX058;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX059;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX060;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX061;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX062;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX063;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX064;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX065;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX066;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX067;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX068;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX069;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX070;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX071;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX072;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX073;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX074;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX075;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX076;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX077;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX078;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX079;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX080;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX081;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX082;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX083;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX084;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX085;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX086;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX087;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX088;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX089;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX090;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX091;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX092;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX093;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX094;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX095;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX096;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX097;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX098;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX099;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX100;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX101;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX102;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX103;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX104;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX105;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX106;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX107;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX108;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX109;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX110;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX111;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX112;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX113;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX114;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX115;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX116;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX117;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX118;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX119;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX120;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX121;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX122;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX123;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX124;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX125;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX126;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX127;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX128;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX129;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX130;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX131;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX132;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX133;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX134;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX135;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX136;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX137;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX138;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX139;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX140;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX141;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX142;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX143;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX144;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX145;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX146;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX147;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX148;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX149;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX150;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX151;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX152;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {"EX153;", 5, false, MENU_REFRESH_TIME, 0, NULL},
    {NULL, 0, false, 0, 0, NULL} // Sentinel value to mark the end of the array
};

// Iterate through the rig_commands array to find the command
static rig_command_t *find_command(const char *cmd) {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (memcmp(rig_commands[i].cmd, cmd, rig_commands[i].len) == 0) {
            return &rig_commands[i];
        }
    }
    return NULL; // Command not found
}

void rig_monitor_task(void *pvParameters) {
    TickType_t last_refresh_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

    while (1) {
        TickType_t start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        TickType_t elapsed_time = start_time - last_refresh_time;

        // ESP_LOGI("RMT", "Elapsed time: %d ms", (int)elapsed_time);
        
        for (int i = 0; rig_commands[i].cmd != NULL; i++) {
            if (rig_commands[i].next_refresh > 0) {
                rig_commands[i].next_refresh -= elapsed_time;
            } else {
                if (!rig_commands[i].pending) {
                    uart_send(rig_commands[i].cmd, SEND_TYPE_MONITOR);
                    rig_commands[i].next_refresh = rig_commands[i].refresh_time;

                    rig_commands[i].pending = true;
                }
            }
        }

        last_refresh_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

        // char buf[128];
        // snprintf(buf, sizeof(buf), "Start: %d, end: %d, elasped: %d", (int)start_time, (int)last_refresh_time, (int)(last_refresh_time - start_time));
        // ESP_LOGI("RMT", "%s", buf);

        vTaskDelay(50 / portTICK_PERIOD_MS); // Delay for a short period
    }
}

static void recv_data_callback(void *context, void *data) {
    recv_result_t *result = (recv_result_t *)data;

    // find the command in rig_commands
    rig_command_t *command = find_command(result->command);
    if (command == NULL) {
        ESP_LOGE(TAG, "Command not found: %s", result->command);
        return;
    }
    if (result->error != ESP_OK) {
        ESP_LOGE(TAG, "Error receiving data: %s", result->data);
        return;
    }
    if (strcmp(result->data, "?;") == 0) {
        ESP_LOGI(TAG, "Error: Command: %s, result: %s", result->command, result->data);
        return;
    }

    command->pending = false;
    command->next_refresh = command->refresh_time;

    if (strncmp(result->data, command->last_value, RECV_BUFFER_SIZE) != 0) {
        strncpy(command->last_value, result->data, RECV_BUFFER_SIZE);

        ESP_LOGI(TAG, "New value, command: %s, result: %s", result->command, result->data);
    }
}

// For commands using the default refresh time, set a random value between 100 and DEFAULT_REFRESH_TIME

void init_rig_commands() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].refresh_time == DEFAULT_REFRESH_TIME || rig_commands[i].refresh_time == MENU_REFRESH_TIME) {
            rig_commands[i].next_refresh = (rand() % rig_commands[i].refresh_time);
        }
        rig_commands[i].last_value = malloc(RECV_BUFFER_SIZE);
    }

    uart_add_recv_observer(recv_data_callback, NULL);

    // create a task to periodically refresh the commands
    if (xTaskCreate(rig_monitor_task, "rig_monitor_task", 2048, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create rig monitor task");
        return;
    }
    ESP_LOGI(TAG, "Rig commands initialized");
}
