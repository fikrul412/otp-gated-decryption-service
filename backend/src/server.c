#include "server.h"
#include "router.h"

#include "mongoose.h"

#include <stdio.h>

static void event_handler(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev != MG_EV_HTTP_MSG)
        return;

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    // Use mg_str() to pack the literal into an mg_str struct to match the function signature
    if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0) {
        mg_http_reply(c, 200, 
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Content-Length: 0\r\n\r\n", "");
        return;
    }

    // Pass standard routes down into your isolated router file
    router(c, hm);
}

void start_server(int port)
{
    struct mg_mgr mgr;

    mg_mgr_init(&mgr);

    char address[64];

    sprintf(address, "http://0.0.0.0:%d", port);

    if (mg_http_listen(&mgr, address, event_handler, NULL) == NULL)
    {
        printf("Failed to listen on %s\n", address);
        return;
    }

    printf("Listening on %s\n", address);

    for (;;)
    {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
}