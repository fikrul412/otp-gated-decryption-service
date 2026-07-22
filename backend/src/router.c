#include "router.h"
#include "controllers/decrypt_controller.h"
#include "controllers/otp_controller.h"
#include "mongoose.h"

#define CORS_HEADERS \
    "Access-Control-Allow-Origin: *\r\n" \
    "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n" \
    "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"

void router(struct mg_connection *c, struct mg_http_message *hm)
{
    if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
        mg_http_reply(c, 200, CORS_HEADERS, "");
        return;
    }

    if (mg_match(hm->uri, mg_str("/decrypt"), NULL)) {
        decrypt_controller(c, hm);
        return;
    }

    if (mg_match(hm->uri, mg_str("/otp"), NULL)) {
        otp_controller(c, hm);
        return;
    }

    mg_http_reply(c, 404, CORS_HEADERS "Content-Type: application/json\r\n", 
                  "{\"status\":\"error\",\"message\":\"Route not found\"}\n");
}