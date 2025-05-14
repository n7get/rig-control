#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "rig_monitor.h"
#include "rig_controller.h"

static bool transmit = false;

#define TAG "RIG_CONTROLLER"

static void handle_transmit(bool value) {
    rm_queue_tx_poll(value);
    transmit = value;
}

static void update_callback(void *context, void *data) {
    char *command = (char *)data;

    if (memcmp(command, "TX", 2) == 0) {
        handle_transmit(command[2] != '0');
        return;
    }
}

void init_rig_controller(void) {
    rig_monitor_add_observers(OBSERVE_UPDATES, update_callback, NULL);
}
