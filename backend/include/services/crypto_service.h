#ifndef CRYPTO_SERVICE_H
#define CRYPTO_SERVICE_H

#include "models/encrypt_request.h"
#include "models/encrypt_response.h"
#include "models/decrypt_request.h"
#include "models/decrypt_response.h"

/**
 * @brief Processes the plaintext encryption and sets the base64 output in response.
 * 
 * @param request Input parameters containing plaintext, key, and iv.
 * @param response Output target struct to populate with the results.
 * @return int 0 on success, non-zero error code on encryption or encoding failure.
 */
int encrypt_service(
    const EncryptRequest *request,
    EncryptResponse *response
);

/**
 * @brief Decodes the base64 ciphertext and processes the decryption.
 * 
 * @param request Input parameters containing base64 ciphertext, key, and iv.
 * @param response Output target struct to populate with the plaintext string.
 * @return int 0 on success, non-zero error code on decoding or decryption failure.
 */
int decrypt_service(
    const DecryptRequest *request,
    DecryptResponse *response
);

#endif // CRYPTO_SERVICE_H