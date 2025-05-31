#pragma once

#include "observer.h"

void ui_init();

void ui_add_observers(observer_callback_t callback, void *context);
void ui_remove_observers(observer_callback_t callback);
void ui_recv(const char *command);
void ui_send_json(const char *json);