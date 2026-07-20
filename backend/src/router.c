#include "router.h"

#include "controllers/health_controller.h"
#include "controllers/encrypt_controller.h"
#include "controllers/decrypt_controller.h"
#include "controllers/otp_controller.h"

// Comprehensive CORS headers ensuring preflight compliance for Svelte/Web clients
#define CORS_HEADERS \
    "Access-Control-Allow-Origin: *\r\n" \
    "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n" \
    "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"

void router(struct mg_connection *c, struct mg_http_message *hm)
{
    // Global Preflight Interceptor: Handle all OPTIONS requests cleanly in one spot
    if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0)
    {
        mg_http_reply(c, 204, CORS_HEADERS, ""); // 204 No Content is ideal for preflight
        return;
    }

    // GET /health
    if (mg_match(hm->uri, mg_str("/health"), NULL))
    {
        if (mg_strcmp(hm->method, mg_str("GET")) != 0)
        {
            mg_http_reply(c, 405, CORS_HEADERS, "Method Not Allowed");
            return;
        }

        health_controller(c, hm);
        return;
    }

    // POST /encrypt
    if (mg_match(hm->uri, mg_str("/encrypt"), NULL))
    {
        if (mg_strcmp(hm->method, mg_str("POST")) != 0)
        {
            mg_http_reply(c, 405, CORS_HEADERS, "Method Not Allowed");
            return;
        }

        encrypt_controller(c, hm);
        return;
    }

    // POST /decrypt
    if (mg_match(hm->uri, mg_str("/decrypt"), NULL))
    {
        if (mg_strcmp(hm->method, mg_str("POST")) != 0)
        {
            mg_http_reply(c, 405, CORS_HEADERS, "Method Not Allowed");
            return;
        }

        decrypt_controller(c, hm);
        return;
    }

    // POST /otp/send
    if (mg_match(hm->uri, mg_str("/otp/send"), NULL))
    {
        if (mg_strcmp(hm->method, mg_str("POST")) != 0)
        {
            mg_http_reply(c, 405, CORS_HEADERS, "Method Not Allowed");
            return;
        }

        otp_controller(c, hm);
        return;
    }

    // 404 Fallback
    mg_http_reply(c, 404, CORS_HEADERS, "Not Found");
}