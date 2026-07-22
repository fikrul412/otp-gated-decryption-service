#include "server.h"
#include "router.h"

#include "mongoose.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
static int setenv(const char *name, const char *value, int overwrite) 
{
    if (!overwrite && getenv(name) != NULL) {
        return 0;
    }
    return _putenv_s(name, value);
}
#endif

static void load_env_file(const char *filename) 
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("[ENV] No %s file found, relying on system environment variables.\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        line[strcspn(line, "\r\n")] = 0;

        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char *key = line;
            char *val = eq + 1;

            if (val[0] == '"' || val[0] == '\'') val++;
            size_t len = strlen(val);
            if (len > 0 && (val[len - 1] == '"' || val[len - 1] == '\'')) {
                val[len - 1] = '\0';
            }

            setenv(key, val, 1);
        }
    }

    fclose(file);
    printf("[ENV] Successfully loaded settings from %s\n", filename);
}

static void event_handler(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        router(c, hm);
    }
}

void start_server(int port)
{
    load_env_file(".env");

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        printf("[ERROR] Failed to initialize libcurl\n");
        return;
    }

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    char address[64];
    snprintf(address, sizeof(address), "http://0.0.0.0:%d", port);

    if (mg_http_listen(&mgr, address, event_handler, NULL) == NULL)
    {
        printf("[ERROR] Failed to listen on %s\n", address);
        mg_mgr_free(&mgr);
        curl_global_cleanup();
        return;
    }

    printf("[SERVER] Listening on %s\n", address);

    for (;;)
    {
        mg_mgr_poll(&mgr, 50);
    }

    mg_mgr_free(&mgr);
    curl_global_cleanup();
}