#pragma once

#include <stdbool.h>
#include "rig.h"

#define POLL_INTERVAL 25

typedef enum {
    RC_COMMAND_NORMAL = 0,
    RC_COMMAND_SPECIAL,
    RC_COMMAND_INVALID,
} rc_recv_command_type;

/**
 * Initialize the rig commands.
 */
void init_rig_commands();

/**
 * rc_is_ready
 * 
 * This function checks if any command in the rig_commands array have been initialized.
 * If any command is not initialized, it sends the command to the CAT interface and updates its status to PENDING_INIT.
 * If all commands are initialized, it returns true.
 */
bool rc_is_ready();

/**
 * rc_scan_for_updates
 * 
 * This function scans through the rig_commands array and checks if any command needs to be updated.
 * If a command needs to be updated it sends it to the CAT interface and sets its status to PENDING.
 */
TickType_t rc_scan_for_updates(TickType_t last_scan_tick);

/**
 * rc_randomize_refresh
 * 
 * This function randomizes the refresh time of all commands
 * The idea is to spread out the refresh time of all commands
 */
void rc_randomize_refresh();

/**
 * rc_handle_special_command
 * 
 * This function handles special commands that are not part 
 * of the CAT command set.
 */
void rc_handle_special_command(const char *cmd_str);

/**
 * rc_recv_command
 * 
 * This function checks if the command is a valid command.
 * If the command is valid, it returns RC_COMMAND_NORMAL.
 * the command is a special command, handle it and return RC_COMMAND_IGNORE.
 * If the command is not valid, it returns RC_COMMAND_INVALID.
 */
rc_recv_command_type rc_recv_command(const char *cmd_str);

/**
 * rc_reset
 * 
 * Force all commands to be re-initialized.
 */
void rc_reset();

/**
 * rc_send_refresh
 * 
 * This function sends the last value of all commands to the network interface.
 */
void rc_send_refresh(void (*notify_callback)(char *));

/**
 * Get command to get the ID of the radio.
 */
const char *rc_id_command();

/**
 * The expected ID of the radio.
 */
const char *rc_is_id();

/**
 * Check if the result is an error indecator from the radio.
 */
bool rc_is_fail(const char *result);

/**
 * rc_set_last_value
 * 
 * This function sets the last value of a command if it is different from the current value.
 */
bool rc_set_last_value(result_buf_t *result, const char *value);

/**
 * Value for monitor not ready.
 */
const char *rc_result_not_ready();

/**
 * Vaalue for monitor ready.
 */
const char *rc_result_ready();

/**
 * Value for monitor encountered an error.
 */
const char *rc_result_error();

/**
 * Value for monitor busy.
 */
const char *rc_result_busy();

/**
 * Value for monitor ping.
 */
const char *rc_result_ping();

/**
 * Special command to refresh the monitor.
 */
const char *rc_refresh_command();

/**
 * Check if the command is for refresh.
 */
bool rc_is_refresh(const char *value);

/**
 * Special command to ping the monitor.
 */
const char *rc_ping_command();
/**
 * Check if the command is for ping.
 */
bool rc_is_ping(const char *value);
