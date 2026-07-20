#include "utils/json_utils.h"

#include <stdlib.h>
#include <stdio.h>

static char *json_get_string(
    cJSON *json,
    const char *key)
{
    cJSON *item =
        cJSON_GetObjectItemCaseSensitive(
            json,
            key
        );

    if (cJSON_IsString(item))
    {
        return item->valuestring;
    }

    return NULL;
}

int parse_encrypt_request(
    cJSON *json,
    EncryptRequest *request)
{
    if (json == NULL || request == NULL)
    {
        return 0;
    }

    request->method =
        json_get_string(json, "method");

    request->message =
        json_get_string(json, "message");

    request->recipient_type =
        json_get_string(json, "recipient_type");

    request->recipient =
        json_get_string(json, "recipient");

    if (!request->method || !request->message)
    {
        return 0;
    }

    return 1;
}

char *serialize_encrypt_response(
    const EncryptResponse *response)
{
    cJSON *json =
        cJSON_CreateObject();

    cJSON_AddStringToObject(
        json,
        "encrypted_data",
        response->encrypted_message ? response->encrypted_message : ""
    );

    cJSON_AddStringToObject(
        json,
        "generated_key",
        response->generated_key ? response->generated_key : ""
    );

    char *result =
        cJSON_PrintUnformatted(json);

    cJSON_Delete(json);

    return result;
}

int parse_decrypt_request(
    cJSON *json,
    DecryptRequest *request)
{
    if (json == NULL || request == NULL)
    {
        return 0;
    }

    request->method =
        json_get_string(json, "method");

    request->encrypted_data =
        json_get_string(json, "encrypted_data");

    request->otp =
        json_get_string(json, "otp");

    request->encryption_key =
        json_get_string(json, "encryption_key");

    if (!request->method || !request->encrypted_data || !request->otp || !request->encryption_key)
    {
        return 0;
    }

    return 1;
}

char *serialize_decrypt_response(
    const DecryptResponse *response)
{
    if (response == NULL)
    {
        return NULL;
    }

    // Print recipient details to the backend console for logging
    printf("[Decrypt] Recipient Type: %s\n", 
           response->recipient_type ? response->recipient_type : "N/A");
    printf("[Decrypt] Recipient: %s\n", 
           response->recipient ? response->recipient : "N/A");

    cJSON *json = cJSON_CreateObject();

    // Only serialize the decrypted message in the response
    cJSON_AddStringToObject(
        json,
        "message",
        response->message ? response->message : ""
    );

    char *result =
        cJSON_PrintUnformatted(json);

    cJSON_Delete(json);

    return result;
}

int parse_otp_request(
    cJSON *json,
    OtpRequest *request)
{
    if (json == NULL || request == NULL)
    {
        return 0;
    }

    request->recipient_type =
        json_get_string(json, "recipient_type");

    request->recipient =
        json_get_string(json, "recipient");

    return 1;
}

char *serialize_otp_response(
    const OtpResponse *response)
{
    cJSON *json =
        cJSON_CreateObject();

    cJSON_AddBoolToObject(
        json,
        "success",
        response->success
    );

    char *result =
        cJSON_PrintUnformatted(json);

    cJSON_Delete(json);

    return result;
}