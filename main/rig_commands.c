#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "cat.h"
#include "rig_commands.h"
#include "esp_log.h"
#include "info.h"

#define TAG "RIG_COMMANDS"

// Updated rig_command_t initialization to include 'len' field, calculated as the length of cmd - 1
static rig_command_t rig_commands[] = {
    {"AC;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"AG0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BC0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BI;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BP00;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BP01;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BY;", 2, INVALID, RESPONSIVE_REFRESH_TIME, 0, {'\0'}},
    {"CN00;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CN01;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO00;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO01;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO02;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO03;", 4, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CS;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CT0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"DA;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"FA;", 2, INVALID, RESPONSIVE_REFRESH_TIME, 0, {'\0'}},
    {"FB;", 2, INVALID, RESPONSIVE_REFRESH_TIME, 0, {'\0'}},
    {"FS;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"FT;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"GT0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    // {"ID;", 2, INVALID, DEFAULT_REFRESH_TIME, 0, {'\0'}},
    {"IF;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"IS0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"KP;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"KR;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"KS;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"LK;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MD0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MG;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"ML0;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"ML1;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MS;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MX;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NA0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NB0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NL0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NR0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"OI;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"OS0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PA0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PC;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PL;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PR0;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PR1;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RA0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RG0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI0;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI3;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI4;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI5;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI6;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI7;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RIA;", 3, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RL0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RT;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"SD;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"SH0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"SQ0;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"TS;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"TX;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"UL;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"VD;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"VG;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"VX;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"XT;", 2, INVALID, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"EX001;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX002;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX003;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX004;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX005;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX006;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX007;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX008;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX009;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX010;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX011;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX012;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX013;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX014;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX015;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX016;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX017;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX018;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX019;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX020;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX021;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX022;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX023;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX024;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX025;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX026;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX027;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX028;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX029;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX030;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX031;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX032;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX033;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX034;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX035;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX036;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX037;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX038;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX039;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX040;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX041;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX042;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX043;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX044;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX045;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX046;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX047;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX048;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX049;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX050;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX051;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX052;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX053;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX054;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX055;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX056;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX057;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX058;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX059;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX060;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX061;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX062;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX063;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX064;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX065;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX066;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX067;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX068;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX069;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX070;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX071;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX072;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX073;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX074;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX075;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX076;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX077;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX078;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX079;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX080;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX081;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX082;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX083;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX084;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX085;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX086;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX087;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX088;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX089;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX090;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX091;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX092;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX093;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX094;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX095;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX096;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX097;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX098;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX099;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX100;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX101;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX102;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX103;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX104;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX105;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX106;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX107;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX108;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX109;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX110;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX111;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX112;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX113;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX114;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX115;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX116;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX117;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX118;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX119;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX120;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX121;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX122;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX123;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX124;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX125;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX126;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX127;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX128;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX129;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX130;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX131;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX132;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX133;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX134;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX135;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX136;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX137;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX138;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX139;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX140;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX141;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX142;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX143;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX144;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX145;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX146;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX147;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX148;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX149;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX150;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX151;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX152;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX153;", 5, INVALID, MENU_REFRESH_TIME, 0, {'\0'}},
    {NULL, 0, UNKNOWN, 0, 0, {'\0'}} // Sentinel value to mark the end of the array
};

/**
 * rig_command_is_ready
 * 
 * This function checks if any command in the rig_commands array have been initialized.
 * If any command is not initialized, it sends the command to the CAT interface and updates its status to PENDING_INIT.
 * If all commands are initialized, it returns true.
 */
bool rig_command_is_ready() {
    bool has_invalid = false;

    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].status == INVALID) {
            has_invalid = true;
            if(cat_send(rig_commands[i].cmd, SEND_TYPE_MONITOR, SEND_PRIORITY_NORMAL) == ESP_ERR_NO_MEM) {
                break;
            }
            // ESP_LOGI(TAG, "Sending initial command: %s", rig_commands[i].cmd);
            rig_commands[i].status = PENDING_INIT;
        }
    }

    if(has_invalid) {
        return false;
    }

    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].status != VADID) {
            return false;
        }
    }
    return true;
}

/**
 * rig_command_scan_for_updates
 * 
 * This function scans through the rig_commands array and checks if any command needs to be updated.
 * If a command needs to be updated it sends it to the CAT interface and sets its status to PENDING.
 */
TickType_t rig_command_scan_for_updates(TickType_t last_scan_tick) {
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t elapsed_ticks = start_tick - last_scan_tick;

    info_t *info = get_info();
    info->polls++;
    info->elapsed_ticks += elapsed_ticks;

    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        switch(rig_commands[i].status) {
            case INVALID:
                ESP_LOGE(TAG, "Command is invalid: %s", rig_commands[i].cmd);
                break;

            case PENDING_INIT:
                ESP_LOGE(TAG, "Command is pending init: %s", rig_commands[i].cmd);
                break;

            case VADID:
                info->valid++;
                if (rig_commands[i].next_refresh <= 0) {
                    if (cat_send(rig_commands[i].cmd, SEND_TYPE_MONITOR, SEND_PRIORITY_NORMAL) == ESP_ERR_NO_MEM) {
                        info->send_queue_full++;
                        break;
                    }
                    info->updates++;
                    rig_commands[i].status = PENDING;
                } else {
                    rig_commands[i].next_refresh -= elapsed_ticks;
                }
                break;

            case PENDING:
                info->pending++;
                break;

            case UNKNOWN:
                ESP_LOGE(TAG, "Unknown command status: %d", rig_commands[i].status);
                break;
        }
    }

    return xTaskGetTickCount();
}

/**
 * rig_command_reset
 * 
 * Force all commands to be re-initialized.
 */
void rig_command_reset() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        rig_commands[i].status = INVALID;
        rig_commands[i].next_refresh = 0;
        rig_commands[i].last_value[0] = '\0';
    }
}

void rig_command_refresh() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        rig_commands[i].last_value[0] = '\0';
    }
}

/**
 * rig_command_set_last_value
 * 
 * This function sets the last value of a command if it is different from the current value.
 * It also updates the status of the command to VALID.
 */
bool rig_command_set_last_value(rig_command_t *cmd, const char *value) {
    // ESP_LOGI(TAG, "rig_command_set_last_value, cmd: %s, status: %d", cmd->cmd, cmd->status);
    cmd->status = VADID;
    cmd->next_refresh = cmd->refresh_time;

    if (strncmp(cmd->last_value, value, RECV_BUFFER_SIZE) != 0) {
        // ESP_LOGI(TAG, "last_value: %s, new_value: %s", cmd->last_value, value);

        strncpy(cmd->last_value, value, RECV_BUFFER_SIZE);

        return true;
    }

    return false;
}

const char *rig_command_id() {
    return "ID;";
}
const char *rig_expected_id() {
    return "ID0670;";
}

bool rig_command_is_fail(const char *result) {
    if (result == NULL) {
        return false;
    }
    return result[0] == '?' && result[1] == ';';
}

// Iterate through the rig_commands array to find the command
rig_command_t *find_command(const char *cmd) {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (memcmp(rig_commands[i].cmd, cmd, rig_commands[i].len) == 0) {
            return &rig_commands[i];
        }
    }
    return NULL;
}

// For commands using the default refresh time, set a random value between 100 and DEFAULT_REFRESH_TIME

void init_rig_commands() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        rig_commands[i].refresh_time = pdMS_TO_TICKS(rig_commands[i].refresh_time);
        // rig_commands[i].next_refresh = (rand() % rig_commands[i].refresh_time);
    }
    // for (int i = 0; rig_commands[i].cmd != NULL; i++) {
    //     ESP_LOGI(TAG, "Command: %s, next_refresh: %d", rig_commands[i].cmd, rig_commands[i].next_refresh);
    // }
}

void log_rig_command(char *tag, rig_command_t *cmd) {
    char *status_str;
    switch (cmd->status) {
        case INVALID:
            status_str = "INVALID";
            break;
        case PENDING_INIT:
            status_str = "PENDING_INIT";
            break;
        case VADID:
            status_str = "VALID";
            break;
        case PENDING:
            status_str = "PENDING";
            break;
        default:
            status_str = "UNKNOWN";
            break;
    }
    ESP_LOGI(tag, "rig_command_t, cmd: %s, len: %d, status: %s, refresh_time: %d, next_refresh: %d, last_value: %s", cmd->cmd, cmd->len, status_str, cmd->refresh_time, cmd->next_refresh, cmd->last_value);
}
