#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "cat.h"
#include "info.h"
#include "rig.h"
#include "rig_commands.h"

#define MIN_REFRESH_TIME pdMS_TO_TICKS(50)
#define FAST_REFRESH_TIME pdMS_TO_TICKS(50)
#define RESPONSIVE_REFRESH_TIME pdMS_TO_TICKS(100)
#define SETTINGS_REFRESH_TIME pdMS_TO_TICKS(1000)
#define MENU_REFRESH_TIME pdMS_TO_TICKS(5000)
#define AUTO_FAST_REFRESH_TIME pdMS_TO_TICKS(5000)

// These flags are used to inilizition of the command values
#define INVALID_F 0x1       // Command not initialized
#define PENDING_INIT_F 0x2  // Command sent, waiting for response

// These flags are used during normal operation
#define VALID_F 0x4         // Command has been initialized
#define PENDING_F 0x8       // Command sent, waiting for response
#define FAST_F 0x10         // Command is set for fast refresh
#define ERROR_F 0x20        // Last response was an error

// These flags for for commands that once they have changed,
// kick into automatic fast refresh mode.  Once the count down
// has expired, the command will go back to normal refresh time.
#define AUTO_FAST_F 0x40    // This flag is never cleared
#define COUNT_DOWN_F 0x80

#define RESET_MASK (AUTO_FAST_F)

// Define a structure to represent each command
typedef struct {
    const char *cmd;       // Command string
    int len;               // Length of the command string (excluding the semicolon)
    uint8_t flags;         // Flags to indicate the status of the command
    int fast_count;        // Fast count for the command
    int refresh_time;      // Refresh time in ticks
    int next_refresh;      // Next refresh time in ticks
    char last_value[RECV_BUFFER_SIZE];      // Last value, initialized to NULL
} rig_command_t;

#define ENHANCED_RIG_COMMAND_REFRESH "@R;"
#define ENHANCED_RIG_COMMAND_PING "@P;"

#define ENHANCED_RIG_RESULT_NOT_READY "!N;"
#define ENHANCED_RIG_RESULT_READY "!R;"
#define ENHANCED_RIG_RESULT_ERROR "!E;"
#define ENHANCED_RIG_RESULT_BUSY "!B;"

#define TAG "RIG_COMMANDS"

// Updated rig_command_t initialization to include 'len' field, calculated as the length of cmd - 1
static rig_command_t rig_commands[] = {
    {"AC;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"AG0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BC0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BI;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BP00;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BP01;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"BY;", 2, INVALID_F, 0, RESPONSIVE_REFRESH_TIME, 0, {'\0'}},
    {"CN00;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CN01;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO00;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO01;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO02;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CO03;", 4, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CS;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"CT0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"DA;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"FA;", 2, INVALID_F, 0, RESPONSIVE_REFRESH_TIME, 0, {'\0'}},
    {"FB;", 2, INVALID_F, 0, RESPONSIVE_REFRESH_TIME, 0, {'\0'}},
    {"FS;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"FT;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"GT0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    // {"ID;", 2, INVALID_F, 0, DEFAULT_REFRESH_TIME, 0, {'\0'}},
    {"IF;", 2, INVALID_F|AUTO_FAST_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"IS0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"KP;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"KR;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"KS;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"LK;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MD0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MG;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"ML0;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"ML1;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MS;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"MX;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NA0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NB0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NL0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"NR0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"OI;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"OS0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PA0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PC;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PL;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PR0;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"PR1;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RA0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RG0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI0;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI3;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI4;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI5;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI6;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RI7;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RIA;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RL0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RM1;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RM4;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RM6;", 3, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"RT;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"SD;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"SH0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"SQ0;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"TS;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"TX;", 2, INVALID_F|AUTO_FAST_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"UL;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"VD;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"VG;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"VX;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"XT;", 2, INVALID_F, 0, SETTINGS_REFRESH_TIME, 0, {'\0'}},
    {"EX001;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX002;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX003;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX004;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX005;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX006;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX007;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX008;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX009;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX010;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX011;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX012;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX013;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX014;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX015;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX016;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX017;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX018;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX019;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX020;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX021;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX022;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX023;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX024;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX025;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX026;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX027;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX028;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX029;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX030;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX031;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX032;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX033;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX034;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX035;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX036;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX037;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX038;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX039;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX040;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX041;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX042;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX043;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX044;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX045;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX046;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX047;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX048;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX049;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX050;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX051;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX052;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX053;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX054;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX055;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX056;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX057;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX058;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX059;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX060;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX061;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX062;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX063;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX064;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX065;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX066;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX067;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX068;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX069;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX070;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX071;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX072;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX073;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX074;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX075;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX076;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX077;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX078;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX079;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX080;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX081;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX082;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX083;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX084;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX085;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX086;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX087;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX088;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX089;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX090;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX091;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX092;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX093;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX094;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX095;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX096;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX097;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX098;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX099;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX100;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX101;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX102;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX103;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX104;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX105;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX106;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX107;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX108;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX109;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX110;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX111;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX112;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX113;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX114;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX115;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX116;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX117;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX118;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX119;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX120;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX121;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX122;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX123;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX124;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX125;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX126;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX127;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX128;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX129;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX130;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX131;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX132;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX133;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX134;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX135;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX136;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX137;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX138;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX139;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX140;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX141;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX142;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX143;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX144;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX145;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX146;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX147;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX148;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX149;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX150;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX151;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX152;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {"EX153;", 5, INVALID_F, 0, MENU_REFRESH_TIME, 0, {'\0'}},
    {NULL, 0, 0, 0, 0, 0, {'\0'}}
};

// Iterate through the rig_commands array to find the command
static rig_command_t *find_command(const char *cmd) {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (memcmp(rig_commands[i].cmd, cmd, rig_commands[i].len) == 0) {
            return &rig_commands[i];
        }
    }
    return NULL;
}

#ifdef LOG_RIG_COMMAND
static void log_rig_command(char *tag, rig_command_t *cmd) {
    char flags_str[9] = {'\0'};
    char *status_str;
    if (cmd->flags & INVALID_F) {
        strcat(flags_str, "I");
    }
    if (cmd->flags & PENDING_INIT_F) {
        strcat(flags_str, "K");
    }
    if (cmd->flags & VALID_F) {
        strcat(flags_str, "V");
    }
    if (cmd->flags & PENDING_F) {
        strcat(flags_str, "P");
    }
    if (cmd->flags & FAST_F) {
        strcat(flags_str, "F");
    }
    if (cmd->flags & AUTO_FAST_F) {
        strcat(flags_str, "A");
    }
    if (cmd->flags & COUNT_DOWN_F) {
        strcat(flags_str, "C");
    }
    if (cmd->flags & ERROR_F) {
        strcat(flags_str, "E");
    }
    ESP_LOGI(tag, "rig_command_t, cmd: %s, len: %d, status: %s, refresh_time: %d, next_refresh: %d, last_value: %s", cmd->cmd, cmd->len, status_str, cmd->refresh_time, cmd->next_refresh, cmd->last_value);
}
#endif

/**
 * First if any commands are uninitialized and send them to the rig.
 * Note: that only the number of commands that will fit in the
 * send queue will be sent.  
 * 
 * If no commands are uninitialized, then check if any
 * responses are pending.  If so, return not readuy.
 * 
 * Once all commands are initialized (VALID_F), then return
 * true.
 */
bool rig_command_is_ready() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].flags & INVALID_F) {
            if(cat_send(rig_commands[i].cmd, SEND_TYPE_MONITOR, SEND_PRIORITY_NORMAL) == ESP_ERR_NO_MEM) {
                return false;
            }
            // ESP_LOGI(TAG, "Sending initial command: %s", rig_commands[i].cmd);
            rig_commands[i].flags &= ~INVALID_F;
            rig_commands[i].flags |= PENDING_INIT_F;
        }
    }

    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (!(rig_commands[i].flags & VALID_F)) {
            return false;
        }
    }
    return true;
}

static bool send_if_update_needed(rig_command_t *cmd, info_t *info, int priority, int elapsed_ticks) {
    if (cmd->flags & PENDING_F) {
        info->pending++;
        return false;
    }

    if (cmd->flags & VALID_F) {
        info->valid++;
        if (cmd->next_refresh <= 0) {
            if (cat_send(cmd->cmd, SEND_TYPE_MONITOR, priority) == ESP_ERR_NO_MEM) {
                return true;
            }
            info->updates++;
            cmd->flags |= PENDING_F;
        } else {
            cmd->next_refresh -= elapsed_ticks;
        }
    }
    return false;
}

// Main loop to check when rig command values need updating.
// This function is called periodically.  When an update is needed,
// send commands to the rig. It uses a tick count to manage
// the timing of the updates.
// It returns the tick count of the last scan.
TickType_t rig_command_scan_for_updates(TickType_t last_scan_tick) {
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t elapsed_ticks = start_tick - last_scan_tick;

    info_t *info = get_info();
    info->polls++;
    info->elapsed_ticks += elapsed_ticks;

    // Process fast commands first
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].flags & FAST_F) {
            if (send_if_update_needed(&rig_commands[i], info, SEND_PRIORITY_HIGH, elapsed_ticks)) {
                info->send_queue_fast_full++;
                break;
            }
        }
    }

    // Process normal commands
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (!(rig_commands[i].flags & FAST_F)) {
            if (send_if_update_needed(&rig_commands[i], info, SEND_PRIORITY_NORMAL, elapsed_ticks)) {
                info->send_queue_full++;
                break;
            }
        }
    }

    return xTaskGetTickCount();
}

rc_recv_command_type rig_command_recv_command(const char *cmd_str) {
    if (cmd_str[0] == '+' || cmd_str[0] == '-') {
        rig_command_t *command = find_command(cmd_str + 1);
        if (command == NULL) {
            ESP_LOGE(TAG, "Command not found: %s", cmd_str + 1);
            return RC_COMMAND_INVALID;
        }

        if (cmd_str[0] == '+') {
            command->flags |= FAST_F;
        } else {
            command->flags &= ~FAST_F;
        }

        return RC_COMMAND_IGNORE;
    }

    rig_command_t *command = find_command(cmd_str);
    if (command == NULL) {
        ESP_LOGE(TAG, "Command not found: %s", cmd_str);
        return RC_COMMAND_INVALID;
    }

    return RC_COMMAND_NORMAL;
}

void rig_command_reset() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        rig_commands[i].flags &= RESET_MASK;
        rig_commands[i].flags |= INVALID_F;
        rig_commands[i].next_refresh = 0;
        rig_commands[i].last_value[0] = '\0';
    }
}

// void rig_command_refresh() {
//     for (int i = 0; rig_commands[i].cmd != NULL; i++) {
//         rig_commands[i].last_value[0] = '\0';
//     }
// }

// A response from the rig has been received.
// Clear pending and set valid.
// If the response is an error, set the error flag.
// If command is fast, set the refresh time to fast.
// If the response is different from the last value and command is auto fast,
// set the fast flag and refresh fast and start the countdown.
// If the response hasn't changed and countdown is active, decrement the countdown.
// If the countdown reaches zero, clear the fast flag.
// If the response is the same return false, otherwise return true.
bool rig_command_set_last_value(result_buf_t *result, const char *value) {
    rig_command_t *cmd = find_command(result->command_buf.data);
    if (cmd == NULL) {
        ESP_LOGE(TAG, "Command not found: %s", result->command_buf.data);
        return false;
    }

    if (!(cmd->flags & PENDING_F) && !(cmd->flags & PENDING_INIT_F)) {
        return false;
    }

    // ESP_LOGI(TAG, "rig_command_set_last_value, cmd: %s, status: %d", cmd->cmd, cmd->status);
    cmd->flags &= ~(PENDING_F | PENDING_INIT_F);
    cmd->flags |= VALID_F;

    if (rig_command_is_fail(value)) {
        cmd->flags |= ERROR_F;
        cmd->flags &= ~FAST_F;
        cmd->last_value[0] = '\0';
        cmd->next_refresh = cmd->refresh_time;
        return false;
    }
    cmd->flags &= ~ERROR_F;
    
    cmd->next_refresh = cmd->flags & FAST_F ? FAST_REFRESH_TIME : cmd->refresh_time;

    if (strncmp(cmd->last_value, value, RECV_BUFFER_SIZE) != 0) {
        // ESP_LOGI(TAG, "last_value: %s, new_value: %s", cmd->last_value, value);

        strncpy(cmd->last_value, value, RECV_BUFFER_SIZE);

        if (cmd->flags & AUTO_FAST_F) {
            cmd->flags |= FAST_F | COUNT_DOWN_F;
            cmd->fast_count = AUTO_FAST_REFRESH_TIME;
        }

        return true;
    } else if (cmd->flags & COUNT_DOWN_F) {
        cmd->fast_count--;
        if (cmd->fast_count <= 0) {
            cmd->flags &= ~(COUNT_DOWN_F | FAST_F);
            cmd->next_refresh = cmd->refresh_time;
        }
    }

    return false;
}

const char *rig_command_id() {
    return "ID;";
}
const char *rig_expected_id() {
    return "ID0670;";
}

const char *rig_command_refresh() {
    return ENHANCED_RIG_COMMAND_REFRESH;
}
bool rig_command_is_refresh(const char *value) {
    return strcmp(value, ENHANCED_RIG_COMMAND_REFRESH) == 0;
}

const char *rig_command_ping() {
    return ENHANCED_RIG_COMMAND_PING;
}
bool rig_command_is_ping(const char *value) {
    return strcmp(value, ENHANCED_RIG_COMMAND_PING) == 0;
}

const char *rig_command_not_ready() {
    return ENHANCED_RIG_RESULT_NOT_READY;
}
const char *rig_command_ready() {
    return ENHANCED_RIG_RESULT_READY;
}
const char *rig_command_error() {
    return ENHANCED_RIG_RESULT_ERROR;
}
const char *rig_command_busy() {
    return ENHANCED_RIG_RESULT_BUSY;
}

bool rig_command_is_fail(const char *result) {
    if (result == NULL) {
        return false;
    }
    return result[0] == '?' && result[1] == ';';
}

void init_rig_commands() {
    // for (int i = 0; rig_commands[i].cmd != NULL; i++) {
    //     rig_commands[i].refresh_time = pdMS_TO_TICKS(rig_commands[i].refresh_time);
    //     // rig_commands[i].next_refresh = (rand() % rig_commands[i].refresh_time);
    // }
    // for (int i = 0; rig_commands[i].cmd != NULL; i++) {
    //     ESP_LOGI(TAG, "Command: %s, next_refresh: %d", rig_commands[i].cmd, rig_commands[i].next_refresh);
    // }
}
