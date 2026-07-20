#include "controllers/decrypt_controller.h"

#include <stdlib.h>
#include <string.h>

#include "mongoose.h"  // Added to define struct mg_connection and mg_http_reply
#include "cJSON.h"

#include "models/decrypt_request.h"
#include "models/decrypt_response.h"

#include "services/crypto_service.h"
#include "utils/json_utils.h"

// Define standard CORS headers to blend with Content-Type strings cleanly
#define CORS_TEXT "Access-Control-Allow-Origin: *\r\nContent-Type: text/plain\r\n"
#define CORS_JSON "Access-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n"

void decrypt_controller(
    struct mg_connection *c,
    struct mg_http_message *hm)
{
    cJSON *json = cJSON_ParseWithLength(
        hm->body.buf,
        hm->body.len
    );

    if (json == NULL)
    {
        mg_http_reply(
            c,
            400,
            CORS_TEXT,
            "Invalid JSON"
        );
        return;
    }

    DecryptRequest request;
    memset(&request, 0, sizeof(DecryptRequest));

    if (!parse_decrypt_request(json, &request))
    {
        cJSON_Delete(json);

        mg_http_reply(
            c,
            400,
            CORS_TEXT,
            "Invalid request fields"
        );
        return;
    }

    DecryptResponse response;
    memset(&response, 0, sizeof(DecryptResponse));

    int status = decrypt_service(&request, &response);
    if (status != 0)
    {
        cJSON_Delete(json);

        mg_http_reply(
            c,
            500,
            CORS_TEXT,
            "Internal Decryption Error"
        );
        return;
    }

    char *result = serialize_decrypt_response(&response);

    if (result != NULL)
    {
        mg_http_reply(
            c,
            200,
            CORS_JSON,
            "%s",
            result
        );
        free(result);
    }
    else
    {
        mg_http_reply(
            c,
            500,
            CORS_TEXT,
            "Serialization Failure"
        );
    }

    // Clean up all dynamically allocated string structures inside the response object
    if (response.message)
    {
        free(response.message);
    }
    if (response.recipient_type)
    {
        free(response.recipient_type);
    }
    if (response.recipient)
    {
        free(response.recipient);
    }

    cJSON_Delete(json);
}