#ifndef RIG_MONITOR_H
#define RIG_MONITOR_H

#include "cat.h"
#include "observer.h"

#define OBSERVE_UPDATES 0x01
#define OBSERVE_COMMANDS 0x02
#define OBSERVE_STATUS 0x04

void rig_monitor_add_observers(uint32_t observed, observer_callback_t callback, void *context);
void rig_monitor_remove_observers(observer_callback_t callback);
void rig_monitor_recv_data(result_buf_t *result);
esp_err_t rig_monitor_send(const char *command, int type);
void init_rig_monitor();

#endif
