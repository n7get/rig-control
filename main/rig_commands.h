#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "rig.h"
#include "freertos/FreeRTOS.h"

#define POLL_INTERVAL 25

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
void rc_set_last_value(response_t *response, void (*notify_callback)(send_type_t type, char *value, bool updated));

/**
 * Value for monitor not ready.
 */
const char *rc_result_not_ready();

/**
 * Vaalue for monitor ready.
 */
const char *rc_result_ready();

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

/**
 * When in TX:
 * Set read meter commands to poll.
 * Inhibit polling the s meter.
 */
void rc_set_tx_poll();
/**
 * When not in TX:
 * Inhibit read meter commands to poll.
 * Allow polling the s meter.
 */
void rc_clear_tx_poll();

esp_err_t rc_setup_command(command_t *command, const char *cmd_str, send_type_t type);

bool rc_commands_are_same(const char *cmd1, const char *cmd2);
uint32_t rc_parse_frequency(const char *value);
bool rc_is_freq_command(const char *value);
bool rc_is_mode_command(const char *value);
bool rc_is_narrow_command(const char *value);
bool rc_is_width_command(const char *value);
bool rc_is_transmit_command(const char *value);
char *rc_freq_command();
bool rc_needs_update(const char *value);
char *rc_make_freq_command(uint32_t frequency);