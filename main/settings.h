#pragma once

#include "cJSON.h"

extern char ap_ssid[32];
extern char ap_password[64];
extern char sta_ssid[32];
extern char sta_password[64];

void load_settings(void);
void st_recv_from_ui(cJSON *json);
