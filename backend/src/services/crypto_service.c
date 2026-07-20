#include "services/crypto_service.h"
#include "crypto/aes.h"
#include "crypto/base64.h"
#include <openssl/rand.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int encrypt_service(const EncryptRequest *request, EncryptResponse *response) {
    if (!request || !response || !request->message) {
        return -1; // Invalid arguments
    }

    unsigned char key[AES_KEY_SIZE];
    unsigned char iv[AES_IV_SIZE];

    if (RAND_bytes(key, AES_KEY_SIZE) != 1 || RAND_bytes(iv, AES_IV_SIZE) != 1) {
        return -2; // Random key/IV generation failure
    }

    // 0. Build JSON payload containing all message metadata
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(payload, "method", request->method ? request->method : "");
    cJSON_AddStringToObject(payload, "message", request->message ? request->message : "");
    cJSON_AddStringToObject(payload, "recipient_type", request->recipient_type ? request->recipient_type : "");
    cJSON_AddStringToObject(payload, "recipient", request->recipient ? request->recipient : "");
    
    char *payload_str = cJSON_PrintUnformatted(payload);
    cJSON_Delete(payload);
    
    if (!payload_str) {
        return -2; // JSON serialization failure
    }

    // 1. Calculate plaintext length, perform AES-256-CBC encryption, then free payload
    int plaintext_len = (int)strlen(payload_str);
    char *raw_ciphertext = aes_encrypt_cbc(payload_str, key, iv);
    
    if (!raw_ciphertext) {
        free(payload_str);
        return -2; // Encryption engine failure
    }

    // Calculate binary length (Plaintext size + PKCS7 padding length)
    int raw_ciphertext_len = plaintext_len + (AES_IV_SIZE - (plaintext_len % AES_IV_SIZE));
    if ((plaintext_len % AES_IV_SIZE) == 0) {
        raw_ciphertext_len = plaintext_len + AES_IV_SIZE; // full-block padding case
    }

    free(payload_str);

    // 2. Convert the raw binary cipher bytes into a web-safe Base64 string
    char *encoded_b64 = base64_encode((const unsigned char*)raw_ciphertext, (size_t)raw_ciphertext_len);
    
    // Always free the temporary raw binary allocation right away
    free(raw_ciphertext);

    if (!encoded_b64) {
        return -3; // Base64 encoding failure
    }

    // Build a single transfer token containing key and IV.
    unsigned char key_iv[AES_KEY_SIZE + AES_IV_SIZE];
    memcpy(key_iv, key, AES_KEY_SIZE);
    memcpy(key_iv + AES_KEY_SIZE, iv, AES_IV_SIZE);

    char *key_iv_b64 = base64_encode(key_iv, sizeof(key_iv));
    if (!key_iv_b64) {
        free(encoded_b64);
        return -4; // Base64 encoding failure for key/iv
    }

    // 3. Populate the output response struct with encrypted data and key only
    response->encrypted_message = encoded_b64;
    response->generated_key = key_iv_b64;
    response->recipient_type = NULL;
    response->recipient = NULL;
    
    return 0; // Success
}

int decrypt_service(const DecryptRequest *request, DecryptResponse *response) {
    if (!request || !response || !request->encrypted_data || !request->encryption_key) {
        return -1; // Invalid arguments
    }

    // 1. Decode incoming Base64 JSON payload back into raw binary bytes
    size_t decoded_len = 0;
    unsigned char *raw_ciphertext = base64_decode(request->encrypted_data, &decoded_len);
    if (!raw_ciphertext) {
        return -2; // Invalid Base64 data provided
    }

    // 1b. Decode the provided key+iv token from the request.
    size_t key_iv_len = 0;
    unsigned char *key_iv = base64_decode(request->encryption_key, &key_iv_len);
    if (!key_iv || key_iv_len != (AES_KEY_SIZE + AES_IV_SIZE)) {
        free(raw_ciphertext);
        if (key_iv) {
            free(key_iv);
        }
        return -3; // Invalid encryption key token
    }

    unsigned char *key = key_iv;
    unsigned char *iv = key_iv + AES_KEY_SIZE;

    // 2. Decrypt the raw binary bytes back into a readable C string
    char *plaintext = aes_decrypt_cbc((const char*)raw_ciphertext, (int)decoded_len, key, iv);
    
    // Free the temporary decoded binary allocations
    free(raw_ciphertext);
    free(key_iv);

    if (!plaintext) {
        return -3; // Decryption failure (likely bad key, corrupted data, or bad IV)
    }

    // 3. Parse the decrypted JSON payload to extract message and recipient info
    cJSON *payload = cJSON_Parse(plaintext);
    if (!payload) {
        free(plaintext);
        return -4; // JSON parse failure
    }

    cJSON *msg_item = cJSON_GetObjectItemCaseSensitive(payload, "message");
    cJSON *recipient_type_item = cJSON_GetObjectItemCaseSensitive(payload, "recipient_type");
    cJSON *recipient_item = cJSON_GetObjectItemCaseSensitive(payload, "recipient");

    // Extract values and store in response
    if (cJSON_IsString(msg_item)) {
        response->message = strdup(msg_item->valuestring);
    } else {
        response->message = strdup("");
    }

    if (cJSON_IsString(recipient_type_item)) {
        response->recipient_type = strdup(recipient_type_item->valuestring);
    } else {
        response->recipient_type = strdup("");
    }

    if (cJSON_IsString(recipient_item)) {
        response->recipient = strdup(recipient_item->valuestring);
    } else {
        response->recipient = strdup("");
    }

    free(plaintext);
    cJSON_Delete(payload);

    return 0; // Success
}