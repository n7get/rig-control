#pragma once

#include "esp_err.h"
#include "observer.h" // For result_buf_t

// Initialize network tasks and queues
esp_err_t network_init(void);
