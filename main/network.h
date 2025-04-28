#ifndef NETWORK_H
#define NETWORK_H

#include "esp_err.h"
#include "observer.h" // For result_buf_t

#define NETWORK_PORT 9911

// Initialize network tasks and queues
esp_err_t network_init(void);

#endif // NETWORK_H
