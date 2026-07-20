#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "cJSON.h"

#include "models/encrypt_request.h"
#include "models/encrypt_response.h"

#include "models/decrypt_request.h"
#include "models/decrypt_response.h"

#include "models/otp_request.h"
#include "models/otp_response.h"

// Encrypt
int parse_encrypt_request(
    cJSON *json,
    EncryptRequest *request
);

char *serialize_encrypt_response(
    const EncryptResponse *response
);

// Decrypt
int parse_decrypt_request(
    cJSON *json,
    DecryptRequest *request
);

char *serialize_decrypt_response(
    const DecryptResponse *response
);

// OTP
int parse_otp_request(
    cJSON *json,
    OtpRequest *request
);

char *serialize_otp_response(
    const OtpResponse *response
);

#endif