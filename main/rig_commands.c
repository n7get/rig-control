#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "cat.h"
#include "info.h"
#include "rig.h"
#include "rig_commands.h"

#define MIN_REFRESH_TIME pdMS_TO_TICKS(50)
#define VERY_FAST_REFRESH_TIME pdMS_TO_TICKS(50)
#define FAST_REFRESH_TIME pdMS_TO_TICKS(100)
#define MEDIUM_REFRESH_TIME pdMS_TO_TICKS(2000)
#define SLOW_REFRESH_TIME pdMS_TO_TICKS(10000)

#define AUTO_FAST_REFRESH_TICKS pdMS_TO_TICKS(10000)

// These flags are used to initialization of the command values
#define INVALID_F 0x1       // Command not initialized
#define PENDING_INIT_F 0x2  // Command sent, waiting for response

// These flags are used during normal operation
#define VALID_F 0x4         // Command has been initialized
#define PENDING_F 0x8       // Command sent, waiting for response
#define FAST_F 0x10         // Command is set for fast refresh

// These flags for for commands that once they have changed,
// kick into automatic fast refresh mode.  Once the count down
// has expired, the command will go back to normal refresh time.
#define AUTO_FAST_F 0x40    // This flag is never cleared
#define CHECK_FAST_UNTIL 0x80

#define POLL_SKIP_F 0x100   // Skip polling for this command
#define SET_ONLY_F 0x200 // Command is only sent, no response expected
#define READ_ONLY_F 0x400 // Command is only read, not polled

#define RESET_MASK (AUTO_FAST_F|POLL_SKIP_F|SET_ONLY_F|READ_ONLY_F)

// Define a structure to represent each command
typedef struct {
    const char *cmd;                    // Command string
    uint8_t len;                        // Length of the command string (excluding the semicolon)
    uint16_t flags;                      // Flags to indicate the status of the command
    TickType_t fast_until;              // Fast count for the command
    int16_t refresh_time;               // Refresh time in ticks
    int16_t next_refresh;               // Next refresh time in ticks
    char last_value[RECV_BUFFER_SIZE];  // Last value, initialized to NULL
} rig_command_t;

#define TAG "RIG_COMMANDS"

// Updated rig_command_t initialization to include 'len' field, calculated as the length of cmd - 1
static rig_command_t rig_commands[] = {
    {"AB", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"AC;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"AG0;", 2, INVALID_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"AM", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"BA", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"BC0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"BD0", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"BI;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"BP00;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"BP01;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"BS", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"BU", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"BY;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"CH0", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"CH1", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"CN00;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"CN01;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"CO00;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"CO01;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"CO02;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"CO03;", 4, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"CS;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"CT0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    // {"DA;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"DN", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    // {"DT0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    // {"DT1;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    // {"DT2;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"ED", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"EK", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"EU", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"FA;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"FB;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"FS;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"FT;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"GT0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    // {"ID;", 2, INVALID_F, 0, DEFAULT_REFRESH_TIME, 0, {'\0'}},
    {"IF;", 2, INVALID_F|AUTO_FAST_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"IS0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"KP;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"KR;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"KS;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"KY", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"LK;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"MA", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"MC;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"MD0;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"MG;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"ML0;", 3, INVALID_F|AUTO_FAST_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"ML1;", 3, INVALID_F|AUTO_FAST_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"MR;", 2, INVALID_F|READ_ONLY_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"MS;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"MX;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"NA0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"NB0;", 2, INVALID_F|AUTO_FAST_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"NL0;", 2, INVALID_F|AUTO_FAST_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"NR0;", 2, INVALID_F|AUTO_FAST_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"OI;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"OS0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"PA0;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"PC;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"PL;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"PR0;", 3, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"PR1;", 3, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"QI", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"QR", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"QS", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"RA0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"RC", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"RD", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"RG0;", 2, INVALID_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"RI0;", 3, INVALID_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"RI3;", 3, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RI4;", 3, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RI5;", 3, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RI6;", 3, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RI7;", 3, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RIA;", 3, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RL0;", 2, INVALID_F|AUTO_FAST_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RM0;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    // {"RM1;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    // {"RM2;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    // {"RM3;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"RM4;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    // {"RM5;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"RM6;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    // {"RM7;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    // {"RM8;", 3, INVALID_F|POLL_SKIP_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"RS;", 2, INVALID_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"RT;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"RU", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"SC", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"SD;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"SH0;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"SM0;", 3, INVALID_F, 0, FAST_REFRESH_TIME, 0, {'\0'}},
    {"SQ0;", 2, INVALID_F|AUTO_FAST_F, 0, MEDIUM_REFRESH_TIME, 0, {'\0'}},
    {"SV", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"TS;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"TX;", 2, INVALID_F, 0, VERY_FAST_REFRESH_TIME, 0, {'\0'}},
    {"UL;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"UP", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"VD;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"VG;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"VM", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"VX;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"XT;", 2, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"ZI", 2, SET_ONLY_F, 0, 0, 0, {'\0'}},
    {"EX001;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX002;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX003;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX004;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX005;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX006;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX007;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX008;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX009;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX010;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX011;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX012;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX013;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX014;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX015;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX016;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX017;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX018;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX019;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX020;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX021;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX022;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX023;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX024;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX025;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX026;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX027;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX028;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX029;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX030;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX031;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX032;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX033;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX034;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX035;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX036;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX037;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX038;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX039;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX040;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX041;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX042;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX043;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX044;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX045;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX046;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX047;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX048;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX049;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX050;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX051;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX052;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX053;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX054;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX055;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX056;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX057;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX058;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX059;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX060;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX061;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX062;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX063;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX064;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX065;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX066;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX067;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX068;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX069;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX070;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX071;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX072;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX073;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX074;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX075;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX076;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX077;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX078;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX079;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX080;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX081;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX082;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX083;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX084;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX085;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX086;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX087;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX088;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX089;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX090;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX091;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX092;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX093;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX094;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX095;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX096;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX097;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX098;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX099;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX100;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX101;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX102;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX103;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX104;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX105;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX106;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX107;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX108;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX109;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX110;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX111;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX112;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX113;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX114;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX115;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX116;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX117;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX118;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX119;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX120;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX121;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX122;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX123;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX124;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX125;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX126;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX127;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX128;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX129;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX130;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX131;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX132;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX133;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX134;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX135;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX136;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX137;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX138;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX139;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX140;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX141;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX142;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX143;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX144;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX145;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX146;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX147;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX148;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX149;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX150;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX151;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX152;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
    {"EX153;", 5, INVALID_F, 0, SLOW_REFRESH_TIME, 0, {'\0'}},
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

esp_err_t rc_setup_command(command_t *command, const char *cmd_str, send_type_t type) {
    size_t command_size = strnlen(cmd_str, SEND_BUFFER_SIZE);
    if (command_size == 0) {
        ESP_LOGE(TAG, "Command is empty");
        return ESP_FAIL;
    }
    if (command_size >= SEND_BUFFER_SIZE) {
        ESP_LOGE(TAG, "Command size %d exceeds buffer size %d", command_size, SEND_BUFFER_SIZE - 1);
        return ESP_FAIL;
    }
    if (cmd_str[command_size - 1] != ';') {
        ESP_LOGE(TAG, "Command must end with a semicolon");
        return ESP_FAIL;
    }

    rig_command_t *rc_cmd = find_command(cmd_str);
    if (rc_cmd == NULL) {
        ESP_LOGE(TAG, "Command %s not found in rig_commands", cmd_str);
        return ESP_FAIL;
    }

    command->type = type;
    switch(type) {
    case SEND_TYPE_COMMAND:
        if (rc_cmd->flags & READ_ONLY_F) {
            ESP_LOGE(TAG, "Command %s is a read-only type and cannot be set", cmd_str);
            return ESP_FAIL;
        }

        strncpy(command->command, cmd_str, SEND_BUFFER_SIZE);
        command->command_len = command_size;
        if (rc_cmd->flags & SET_ONLY_F) {
            command->read[0] = '\0';
            command->read_len = 0;
        } else {
            // Add a read command to get the new value into the event handlers
            strcpy(command->read, rc_cmd->cmd);
            command->read_len = strlen(command->read);
        }
        break;

    case SEND_TYPE_READ:
    case SEND_TYPE_POLL:
        if (rc_cmd->flags & SET_ONLY_F) {
            ESP_LOGE(TAG, "Command %s is a command-only type and cannot be read", cmd_str);
            return ESP_FAIL;
        }

        strncpy(command->read, cmd_str, SEND_BUFFER_SIZE);
        command->read_len = strlen(cmd_str);
        break;

    default:
        ESP_LOGE(TAG, "Invalid command type %d", type);
        return ESP_FAIL;
    }

    return ESP_OK;
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
    if (cmd->flags & CHECK_FAST_UNTIL) {
        strcat(flags_str, "C");
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
bool rc_is_ready() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].flags & INVALID_F) {
            command_t command;
            memset(&command, 0, sizeof(command_t));
            command.type = SEND_TYPE_POLL;
            strcpy(command.read, rig_commands[i].cmd);
            command.read_len = strlen(command.read);

            if(cat_queue_command(&command, SEND_PRIORITY_NORMAL) == ESP_ERR_NO_MEM) {
                return false;
            }

            // ESP_LOGI(TAG, "Sending initial command: %s", rig_commands[i].cmd);
            rig_commands[i].flags &= ~INVALID_F;
            rig_commands[i].flags |= PENDING_INIT_F;
        }
    }

    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (!(rig_commands[i].flags & (SET_ONLY_F|READ_ONLY_F))) {
            if (!(rig_commands[i].flags & VALID_F)) {
                return false;
            }
        }
    }
    return true;
}

static bool send_if_update_needed(rig_command_t *cmd, info_t *info, int priority, int elapsed_ticks) {
    if (cmd->flags & (POLL_SKIP_F|SET_ONLY_F|READ_ONLY_F)) {
        return false;
    }
    if (cmd->flags & PENDING_F) {
        info->rc_pending++;
        return false;
    }

    if (cmd->flags & VALID_F) {
        info->rc_valid++;
        if (cmd->next_refresh <= 0) {
            command_t command;
            memset(&command, 0, sizeof(command_t));
            command.type = SEND_TYPE_POLL;
            strcpy(command.read, cmd->cmd);
            command.read_len = strlen(command.read);

            if (cat_queue_command(&command, priority) == ESP_ERR_NO_MEM) {
                return true;
            }
            info->rc_updates++;
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
TickType_t rc_scan_for_updates(TickType_t last_scan_tick) {
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t elapsed_ticks = start_tick - last_scan_tick;

    info_t *info = get_info();
    info->rc_polls++;
    info->rc_elapsed_ticks += elapsed_ticks;

    // Process fast commands first
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].flags & FAST_F) {
            if (send_if_update_needed(&rig_commands[i], info, SEND_PRIORITY_HIGH, elapsed_ticks)) {
                info->cat_queue_fast_full++;
                break;
            }
            // ESP_LOGI(TAG, "Fast command: %s", rig_commands[i].cmd);
        }
    }

    // Process normal commands
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (!(rig_commands[i].flags & FAST_F)) {
            if (send_if_update_needed(&rig_commands[i], info, SEND_PRIORITY_NORMAL, elapsed_ticks)) {
                info->cat_queue_full++;
                break;
            }
        }
    }

    return xTaskGetTickCount();
}

void rc_randomize_refresh() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        rig_commands[i].next_refresh = esp_random() % rig_commands[i].refresh_time;
    }
}

void rc_reset() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (!(rig_commands[i].flags & (SET_ONLY_F|READ_ONLY_F))) {
            rig_commands[i].flags &= RESET_MASK;
            rig_commands[i].flags |= INVALID_F;
            rig_commands[i].next_refresh = 0;
            rig_commands[i].last_value[0] = '\0';
        }
    }
}

static void prepare_notify(char *buffer, const char *read_command, const char *value) {
    if (rc_is_fail(value)) {
        strcpy(buffer, "?");
        strncpy(buffer + 1, read_command, RECV_BUFFER_SIZE - 2);
    } else {
        strncpy(buffer, value, RECV_BUFFER_SIZE - 1);
    }
}

void rc_send_refresh(void (*notify_callback)(char *)) {
    char buffer[RECV_BUFFER_SIZE];

    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (!(rig_commands[i].flags & (SET_ONLY_F|READ_ONLY_F)) && rig_commands[i].flags & VALID_F) {
            prepare_notify(buffer, rig_commands[i].cmd, rig_commands[i].last_value);
            notify_callback(buffer);
        }
    }
}

// A response from the rig has been received.
// Clear pending and set valid.
// If the response is an error, set the error flag.
// If command is fast, set the refresh time to fast.
// If the response is different from the last value and command is auto fast,
// set the fast flag and refresh fast and start the countdown.
// If the response hasn't changed and countdown is active, decrement the countdown.
// If the countdown reaches zero, clear the fast flag.
// If the response is the same return false, otherwise return true.
void rc_set_last_value(response_t *response, void (*notify_callback)(send_type_t type, char *value, bool updated)) {
    rig_command_t *cmd = find_command(response->read);
    if (cmd == NULL) {
        ESP_LOGE(TAG, "Command not found: %s", response->read);
        return;
    }

    char buffer[RECV_BUFFER_SIZE];
    switch(response->type) {
    case SEND_TYPE_COMMAND:
        prepare_notify(buffer, response->read, response->response);
        notify_callback(response->type, buffer, true);
        break;

    case SEND_TYPE_READ:
        prepare_notify(buffer, response->read, response->response);
        notify_callback(response->type, buffer, true);
        return;

    case SEND_TYPE_POLL:
        if (!(cmd->flags & PENDING_F) && !(cmd->flags & PENDING_INIT_F)) {
            return;
        }

        cmd->flags &= ~(PENDING_F | PENDING_INIT_F);
        cmd->flags |= VALID_F;

        cmd->next_refresh = cmd->flags & FAST_F ? VERY_FAST_REFRESH_TIME : cmd->refresh_time;
        break;
    }

    if (strncmp(cmd->last_value, response->response, RECV_BUFFER_SIZE) != 0) {
        // ESP_LOGI(TAG, "last_value: %s, new_value: %s", cmd->last_value, response->response);

        strncpy(cmd->last_value, response->response, RECV_BUFFER_SIZE);

        if (cmd->flags & AUTO_FAST_F) {
            cmd->flags |= FAST_F | CHECK_FAST_UNTIL;
            cmd->fast_until = xTaskGetTickCount() + AUTO_FAST_REFRESH_TICKS;
            // ESP_LOGI(TAG, "Auto fast command: %s", cmd->cmd);
        }

        if (response->type == SEND_TYPE_POLL) {
            prepare_notify(buffer, cmd->cmd, cmd->last_value);
            notify_callback(response->type, buffer, true);
        }
    } else if (cmd->flags & CHECK_FAST_UNTIL) {
        if (xTaskGetTickCount() > cmd->fast_until) {
            cmd->flags &= ~(FAST_F | CHECK_FAST_UNTIL);
            cmd->next_refresh = cmd->refresh_time;
            // ESP_LOGI(TAG, "Auto fast command expired: %s", cmd->cmd);
        }
    }
}

const char *rc_id_command() {
    return "ID;";
}
const char *rc_is_id() {
    return "ID0670;";
}

bool rc_is_fail(const char *response) {
    if (response == NULL) {
        return false;
    }
    return response[0] == '?' && response[1] == ';';
}

void rc_set_tx_poll() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].cmd[0] == 'R' && rig_commands[i].cmd[1] == 'M') {
            rig_commands[i].flags &= ~POLL_SKIP_F;
            rig_commands[i].flags |= FAST_F;
        }
        if (rig_commands[i].cmd[0] == 'S' && rig_commands[i].cmd[1] == 'M') {
            rig_commands[i].flags |= POLL_SKIP_F;
        }
    }
}

void rc_clear_tx_poll() {
    for (int i = 0; rig_commands[i].cmd != NULL; i++) {
        if (rig_commands[i].cmd[0] == 'R' && rig_commands[i].cmd[1] == 'M') {
            rig_commands[i].flags &= ~FAST_F;
            rig_commands[i].flags |= POLL_SKIP_F;
        }
        if (rig_commands[i].cmd[0] == 'S' && rig_commands[i].cmd[1] == 'M') {
            rig_commands[i].flags &= ~POLL_SKIP_F;
        }
    }
}

bool rc_commands_are_same(const char *cmd1, const char *cmd2) {
    if (cmd1 == NULL || cmd2 == NULL) {
        return false;
    }
    rig_command_t *rc1 = cmd1[0] == '?' ? find_command(cmd1 + 1) : find_command(cmd1);
    rig_command_t *rc2 = cmd2[0] == '?' ? find_command(cmd2 + 1) : find_command(cmd2);
    if (rc1 == NULL || rc2 == NULL) {
        return false;
    }
    return rc1 == rc2;
}

uint32_t rc_parse_frequency(const char *value) {
    // if (value[0] == 'F' && value[1] == 'A') {
    //     uint32_t freq = strtoul(value + 2, NULL, 10);
    //     return freq;
    // }
    if (value[0] == 'I' && value[1] == 'F') {
        char freq_str[10];
        memcpy(freq_str, value + 5, 9);
        freq_str[9] = '\0';

        return strtoul(freq_str, NULL, 10);
    }
    return 0;
}
bool rc_is_mode_command(const char *value) {
    return (value[0] == 'M' && value[1] == 'D');
}
bool rc_is_narrow_command(const char *value) {
    return (value[0] == 'N' && value[1] == 'A');
}
bool rc_is_width_command(const char *value) {
    return (value[0] == 'S' && value[1] == 'H');
}

char *rc_make_freq_command(uint32_t frequency) {
    char *freq_cmd = malloc(SEND_BUFFER_SIZE);
    snprintf(freq_cmd, SEND_BUFFER_SIZE, "FA%09lu;", frequency);
    return freq_cmd;
}

char *rc_freq_command() {
    return "IF;";
}

bool rc_needs_update(const char *value) {
    rig_command_t *cmd = find_command(value);
    if (cmd == NULL) {
        ESP_LOGE(TAG, "Command not found: %s", value);
        return false;
    }
    if (cmd->flags & (SET_ONLY_F | READ_ONLY_F)) {
        return false;
    }
    return strcmp(cmd->last_value, value) != 0;
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
