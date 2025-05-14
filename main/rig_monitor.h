#pragma once

#include "cat.h"
#include "observer.h"

#define OBSERVE_UPDATES 0x01
#define OBSERVE_COMMANDS 0x02
#define OBSERVE_STATUS 0x04

/**
 * @brief Initialize the rig monitor.
 */
void init_rig_monitor();

/**
 * @brief Queue a command to the rig monitor.
 * 
 * @param command The command to send.
 * @param type The type of the command.
 * 
 * If the command is a special command, it will be sent handled by the rig monitor.
 * If the command is a normal command, it will be sent to the CAT interface.
 * 
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t rm_queue_command(const char *command, send_type_t type);

/**
 * @brief Queue result from CAT interface to the rig monitor.
 * 
 * @param result The result buffer to queue.
 */
void rm_queue_response(response_t *result);

/**
 * @brief Add observers to the output of rig monitor.
 * 
 * @param observed The type of data to observe (updates, commands, status).
 * @param callback The callback function to call when data is received.
 * @param context The context to pass to the callback function.
 */
void rig_monitor_add_observers(uint32_t observed, observer_callback_t callback, void *context);

/**
 * @brief Remove observers from the output of rig monitor.
 * 
 * @param callback The callback function to remove.
 */
void rig_monitor_remove_observers(observer_callback_t callback);
