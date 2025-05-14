#pragma once

#include "rig.h"

#define WS_BUFFER_SIZE 64

void ws_init();

void ws_send(const char *message);
