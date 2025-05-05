#pragma once

#include <stdbool.h>
#include "rig.h"

#define POLL_INTERVAL 25

typedef enum {
    RC_COMMAND_NORMAL = 0,
    RC_COMMAND_IGNORE,
    RC_COMMAND_INVALID,
} rc_recv_command_type;

/**
 * Initialize the rig commands.
 */
void init_rig_commands();

/**
 * rig_command_is_ready
 * 
 * This function checks if any command in the rig_commands array have been initialized.
 * If any command is not initialized, it sends the command to the CAT interface and updates its status to PENDING_INIT.
 * If all commands are initialized, it returns true.
 */
bool rig_command_is_ready();

/**
 * rig_command_scan_for_updates
 * 
 * This function scans through the rig_commands array and checks if any command needs to be updated.
 * If a command needs to be updated it sends it to the CAT interface and sets its status to PENDING.
 */
TickType_t rig_command_scan_for_updates(TickType_t last_scan_tick);

/**
 * rig_command_recv_command
 * 
 * This function checks if the command is a valid command.
 * If the command is valid, it returns RC_COMMAND_NORMAL.
 * the command is a special command, handle it and return RC_COMMAND_IGNORE.
 * If the command is not valid, it returns RC_COMMAND_INVALID.
 */
rc_recv_command_type rig_command_recv_command(const char *cmd_str);

/**
 * rig_command_reset
 * 
 * Force all commands to be re-initialized.
 */
void rig_command_reset();

// void rig_command_refresh();

/**
 * Get command to get the ID of the radio.
 */
const char *rig_command_id();

/**
 * The expected ID of the radio.
 */
const char *rig_expected_id();

/**
 * Check if the result is an error indecator from the radio.
 */
bool rig_command_is_fail(const char *result);

/**
 * rig_command_set_last_value
 * 
 * This function sets the last value of a command if it is different from the current value.
 */
bool rig_command_set_last_value(result_buf_t *result, const char *value);

/**
 * Value for monitor not ready.
 */
const char *rig_command_result_not_ready();

/**
 * Vaalue for monitor ready.
 */
const char *rig_command_result_ready();

/**
 * Value for monitor encountered an error.
 */
const char *rig_command_result_error();

/**
 * Value for monitor busy.
 */
const char *rig_command_result_busy();

/**
 * Value for monitor ping.
 */
const char *rig_command_result_ping();

/**
 * Special command to refresh the monitor.
 */
const char *rig_command_refresh();

/**
 * Check if the command is for refresh.
 */
bool rig_command_is_refresh(const char *value);

/**
 * Special command to ping the monitor.
 */
const char *rig_command_ping();
/**
 * Check if the command is for ping.
 */
bool rig_command_is_ping(const char *value);
