#include "http.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <esp_vfs.h>
#include "esp_littlefs.h"

static const char *TAG = "HTTP";
static httpd_handle_t server = NULL;

httpd_handle_t *get_http_server(void) {
    return &server;
}

void register_handler(const httpd_uri_t *uri) {
    esp_err_t err = httpd_register_uri_handler(server, uri);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Registered URI: %s", uri->uri);
    } else {
        ESP_LOGE(TAG, "Failed to register URI: %s", uri->uri);
    }
}

void register_html_page(const char *uri, httpd_method_t method, esp_err_t handler(httpd_req_t *)) {
    if (server == NULL) {
        ESP_LOGE(TAG, "Web server is not running. Cannot register URI.");
        return;
    }

    httpd_uri_t page_uri = {
        .uri = uri,
        .method = method,
        .handler = handler};

    register_handler(&page_uri);
}

// static esp_err_t redirect_handler(httpd_req_t *req) {
//     ESP_LOGI(TAG, "Received redirect request: %s", req->uri);
//     httpd_resp_set_status(req, "307 Temporary Redirect");
//     httpd_resp_set_hdr(req, "Location", "/settings.html");
//     // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
//     httpd_resp_send(req, "Redirect", HTTPD_RESP_USE_STRLEN);
//     return ESP_OK;
// }

// HTTP Error (404) Handler - Redirects all requests to the root page
// esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
// {
//     // Set status
//     httpd_resp_set_status(req, "302 Temporary Redirect");
//     // Redirect to the "/" root directory
//     httpd_resp_set_hdr(req, "Location", "/");
//     // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
//     httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

//     ESP_LOGI(TAG, "Redirecting to root");
//     return ESP_OK;
// }

static esp_err_t file_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Received file request: %s", req->uri);

    char filepath[ESP_VFS_PATH_MAX + 128] = HTML_MOUNT_POINT;
    strncat(filepath, req->uri, sizeof(filepath) - strlen(filepath) - 1);
    ESP_LOGI(TAG, "File path: %s", filepath);

    int filepath_len = strlen(filepath);
    if (strcmp(filepath + filepath_len - 3, ".js") == 0) {
        httpd_resp_set_type(req, "application/javascript");
    } else if (strcmp(filepath + filepath_len - 4, ".css") == 0) {
        httpd_resp_set_type(req, "text/css");
    } else if (strcmp(filepath + filepath_len - 5, ".html") == 0) {
        httpd_resp_set_type(req, "text/html");
    } else if (strcmp(filepath + filepath_len - 4, ".png") == 0) {
        httpd_resp_set_type(req, "image/png");
    } else if (strcmp(filepath + filepath_len - 4, ".jpg") == 0) {
        httpd_resp_set_type(req, "image/jpeg");
    } else {
        ESP_LOGE(TAG, "Unsupported file type: %s", filepath);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Unsupported file type");
        return ESP_FAIL;
    }

    FILE *file = fopen(filepath, "r");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file: %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    char buffer[1024];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (httpd_resp_send_chunk(req, buffer, read_bytes) != ESP_OK) {
            fclose(file);
            ESP_LOGE(TAG, "File sending failed");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
            return ESP_FAIL;
        }
    }

    fclose(file);
    httpd_resp_send_chunk(req, NULL, 0); // End response
    return ESP_OK;
}

static void register_files(char *fs_path) {
    ESP_LOGI(TAG, "Filesystem path: %s", fs_path);

    DIR *dir = opendir(fs_path);
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open HTML directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char filepath[ESP_VFS_PATH_MAX + 128];
        strncpy(filepath, fs_path, sizeof(filepath) - 1);
        strncat(filepath, "/", sizeof(filepath) - strlen(filepath) - 1);
        strncat(filepath, entry->d_name, sizeof(filepath) - strlen(filepath) - 1);

        if (entry->d_type == DT_DIR) {
            register_files(filepath);
        }
        else if (entry->d_type == DT_REG) {
            register_html_page(filepath + strlen(HTML_MOUNT_POINT), HTTP_GET, file_handler);
        }
    }

    closedir(dir);
}

bool start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Web server started");

        // register_html_page("/", HTTP_GET, redirect_handler);
        // httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);

        ESP_LOGI(TAG, "Reading HTML contents...");

        register_files(HTML_MOUNT_POINT);

        ESP_LOGI(TAG, "Finished listing HTML contents");

        return true;
    } else {
        ESP_LOGE(TAG, "Failed to start web server");
        return false;
    }
}

void stop_webserver(void) {
    if (server != NULL) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "Web server stopped");
    }
}
