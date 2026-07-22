#include "controllers/otp_controller.h"
#include "services/otp_service.h"
#include "cJSON.h"

#define CORS_HEADERS \
    "Access-Control-Allow-Origin: *\r\n" \
    "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n" \
    "Access-Control-Allow-Headers: Content-Type, Authorization\r\n" \
    "Content-Type: application/json\r\n"

void otp_controller(struct mg_connection *c, struct mg_http_message *hm)
{
    cJSON *json = cJSON_ParseWithLength(hm->body.buf, hm->body.len);
    if (!json) {
        mg_http_reply(c, 400, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Invalid JSON\"}\n");
        return;
    }

    cJSON *session_item = cJSON_GetObjectItemCaseSensitive(json, "session_id");
    cJSON *otp_item = cJSON_GetObjectItemCaseSensitive(json, "otp");

    if (!cJSON_IsString(session_item) || !cJSON_IsString(otp_item)) {
        cJSON_Delete(json);
        mg_http_reply(c, 400, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Missing session_id or otp\"}\n");
        return;
    }

    char *plaintext = otp_verify_session(session_item->valuestring, otp_item->valuestring);
    cJSON_Delete(json);

    if (!plaintext) {
        mg_http_reply(c, 401, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Invalid or expired OTP code\"}\n");
        return;
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "plaintext", plaintext);
    free(plaintext);

    char *response_body = cJSON_PrintUnformatted(response_json);
    cJSON_Delete(response_json);

    mg_http_reply(c, 200, CORS_HEADERS, "%s", response_body);
    free(response_body);
}