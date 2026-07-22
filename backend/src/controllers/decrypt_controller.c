#include "controllers/decrypt_controller.h"
#include "services/crypto_service.h"
#include "services/otp_service.h"
#include "services/email_service.h"
#include "cJSON.h"

#define CORS_HEADERS \
    "Access-Control-Allow-Origin: *\r\n" \
    "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n" \
    "Access-Control-Allow-Headers: Content-Type, Authorization\r\n" \
    "Content-Type: application/json\r\n"

void decrypt_controller(struct mg_connection *c, struct mg_http_message *hm)
{
    cJSON *json = cJSON_ParseWithLength(hm->body.buf, hm->body.len);
    if (!json) {
        mg_http_reply(c, 400, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Invalid JSON\"}\n");
        return;
    }

    cJSON *password_item = cJSON_GetObjectItemCaseSensitive(json, "password");
    cJSON *package_item = cJSON_GetObjectItemCaseSensitive(json, "encrypted_package");

    if (!cJSON_IsString(password_item) || (!cJSON_IsString(package_item) && !cJSON_IsObject(package_item))) {
        cJSON_Delete(json);
        mg_http_reply(c, 400, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Missing password or encrypted_package\"}\n");
        return;
    }

    char *package_str = NULL;
    if (cJSON_IsObject(package_item)) {
        package_str = cJSON_PrintUnformatted(package_item);
    } else {
        package_str = strdup(package_item->valuestring);
    }

    char *plaintext = NULL;
    char *recipient_type = NULL;
    char *recipient = NULL;

    int dec_res = decrypt_package(password_item->valuestring, package_str, &plaintext, &recipient_type, &recipient);
    free(package_str);
    cJSON_Delete(json);

    if (dec_res != 0) {
        mg_http_reply(c, 401, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Decryption failed or invalid password\"}\n");
        return;
    }

    char *session_id = otp_create_session(recipient_type, recipient, plaintext);
    free(plaintext); 

    if (!session_id) {
        free(recipient_type);
        free(recipient);
        mg_http_reply(c, 500, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Failed to create OTP session\"}\n");
        return;
    }

    char *otp_code = otp_get_code_by_session_id(session_id);
    if (!otp_code) {
        free(session_id);
        free(recipient_type);
        free(recipient);
        mg_http_reply(c, 500, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Failed to fetch session OTP\"}\n");
        return;
    }

    int email_res = send_otp_email(recipient, otp_code);
    free(otp_code);
    free(recipient_type);
    free(recipient);

    if (email_res != 0) {
        free(session_id);
        mg_http_reply(c, 500, CORS_HEADERS, "{\"status\":\"error\",\"message\":\"Failed to dispatch OTP email\"}\n");
        return;
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "session_id", session_id);
    free(session_id);

    char *response_body = cJSON_PrintUnformatted(response_json);
    cJSON_Delete(response_json);

    mg_http_reply(c, 200, CORS_HEADERS, "%s", response_body);
    free(response_body);
}