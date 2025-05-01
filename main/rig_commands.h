// generate rig_commands.h file
#ifndef RIG_COMMANDS_H
#define RIG_COMMANDS_H

#include <stdbool.h>

#define POLL_INTERVAL 50
#define MIN_REFRESH_TIME 50
#define RESPONSIVE_REFRESH_TIME 100
#define SETTINGS_REFRESH_TIME 1000
#define MENU_REFRESH_TIME 5000
#define SEND_BUFFER_SIZE 64
#define RECV_BUFFER_SIZE 64

typedef enum {
    INVALID = 0,
    PENDING_INIT,
    PENDING,
    VADID,
    UNKNOWN,
} command_status_t;

// Define a structure to represent each command
typedef struct {
    const char *cmd;       // Command string
    command_status_t len;  // Length of the command string (excluding the semicolon)
    int status;            // Whether the command is pending
    int refresh_time;      // Refresh time in ticks
    int next_refresh;      // Next refresh time in ticks
    char last_value[RECV_BUFFER_SIZE];      // Last value, initialized to NULL
} rig_command_t;

#define ENHANCED_RIG_COMMAND_REFRESH "@R;"
#define ENHANCED_RIG_COMMAND_ENHANCED_MODE "@E;"

#define ENHANCED_RIG_RESULT_NOT_READY "!N;"
#define ENHANCED_RIG_RESULT_READY "!R;"
#define ENHANCED_RIG_RESULT_ERROR "!E;"
#define ENHANCED_RIG_RESULT_BUSY "!B;"

void init_rig_commands();
bool rig_command_is_ready();
TickType_t rig_command_scan_for_updates(TickType_t last_scan_tick);
void rig_command_reset();
void rig_command_refresh();
const char *rig_command_id();
const char *rig_expected_id();
bool rig_command_is_fail(const char *result);
rig_command_t *find_command(const char *cmd);
bool rig_command_set_last_value(rig_command_t *cmd, const char *value);
void log_rig_command(char *tag, rig_command_t *cmd);

#endif// RIG_COMMANDS_H