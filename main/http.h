#pragma once

#include "esp_err.h"
#include <esp_http_server.h>

#define HTML_MOUNT_POINT "/html"

bool start_webserver(void);
void stop_webserver(void);
httpd_handle_t *get_http_server(void);
void register_handler(const httpd_uri_t *uri);
void register_html_page(const char *uri, httpd_method_t method, esp_err_t handler(httpd_req_t *));
