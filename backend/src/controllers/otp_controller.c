#include "controllers/otp_controller.h"

#include <stdlib.h>

#include "mongoose.h"  // Added to define struct mg_connection and mg_http_reply
#include "cJSON.h"

#include "models/otp_request.h"
#include "models/otp_response.h"

#include "services/otp_service.h"
#include "utils/json_utils.h"

// Define standard CORS headers to blend with Content-Type strings cleanly
#define CORS_TEXT "Access-Control-Allow-Origin: *\r\nContent-Type: text/plain\r\n"
#define CORS_JSON "Access-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n"

void otp_controller(
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

    OtpRequest request;

    if (!parse_otp_request(json, &request))
    {
        cJSON_Delete(json);

        mg_http_reply(
            c,
            400,
            CORS_TEXT,
            "Invalid request"
        );
        return;
    }

    OtpResponse response =
        otp_service(&request);

    char *result =
        serialize_otp_response(&response);

    // Added CORS headers to the successful response profile
    mg_http_reply(
        c,
        200,
        CORS_JSON,
        "%s",
        result
    );

    free(result);
    cJSON_Delete(json);
}