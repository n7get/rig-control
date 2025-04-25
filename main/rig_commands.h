// generate rig_commands.h file
#ifndef RIG_COMMANDS_H
#define RIG_COMMANDS_H

#include <stdbool.h>

#define DEFAULT_REFRESH_TIME 1000
#define MENU_REFRESH_TIME 5000

// Define a structure to represent each command
typedef struct {
    const char *cmd;       // Command string
    int len;               // Length of the command string (excluding the semicolon)
    bool pending;          // Whether the command is pending
    int refresh_time;      // Refresh time in milliseconds
    int next_refresh;      // Next refresh time in milliseconds
    char *last_value;      // Last value, initialized to NULL
} rig_command_t;

void init_rig_commands();

#endif// RIG_COMMANDS_H