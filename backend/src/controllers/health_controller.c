#include "controllers/health_controller.h"
#include "mongoose.h"  // Added to define struct mg_connection and mg_http_reply

void health_controller(
    struct mg_connection *c,
    struct mg_http_message *hm)
{
    (void) hm;

    mg_http_reply(
        c, 
        200, 
        "Access-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n", 
        "{ \"status\": \"ok\" }"
    );
}