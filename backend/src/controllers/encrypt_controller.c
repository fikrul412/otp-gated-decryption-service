#include "controllers/encrypt_controller.h"

#include <stdlib.h>
#include <string.h>

#include "mongoose.h"  
#include "cJSON.h"

#include "models/encrypt_request.h"
#include "models/encrypt_response.h"

#include "services/crypto_service.h"
#include "utils/json_utils.h"

// Define standard CORS headers to blend with Content-Type strings cleanly
#define CORS_TEXT "Access-Control-Allow-Origin: *\r\nContent-Type: text/plain\r\n"
#define CORS_JSON "Access-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n"

void encrypt_controller(struct mg_connection *c, struct mg_http_message *hm)
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

    EncryptRequest request;
    memset(&request, 0, sizeof(EncryptRequest)); 

    if (!parse_encrypt_request(json, &request))
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

    EncryptResponse response;
    memset(&response, 0, sizeof(EncryptResponse));

    int status = encrypt_service(&request, &response);
    if (status != 0)
    {
        cJSON_Delete(json);
        mg_http_reply(
            c,
            500,
            CORS_TEXT,
            "Internal Encryption Error"
        );
        return;
    }

    char *result = serialize_encrypt_response(&response);

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

    // Dynamic heap allocations released safely below
    if (response.encrypted_message)
    {
        free(response.encrypted_message);
    }
    if (response.generated_key)
    {
        free(response.generated_key);
    }
    
    cJSON_Delete(json);
}