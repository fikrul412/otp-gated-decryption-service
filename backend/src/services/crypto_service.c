#include "services/crypto_service.h"
#include "crypto/aes.h"
#include "crypto/base64.h"

#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int decrypt_package(const char *password, const char *encrypted_package, char **plaintext, char **recipient_type, char **recipient) 
{
    if (!password || !encrypted_package || !plaintext || !recipient_type || !recipient) {
        return -1;
    }

    cJSON *package_json = cJSON_Parse(encrypted_package);
    if (!package_json) {
        return -2;
    }

    cJSON *salt_item = cJSON_GetObjectItemCaseSensitive(package_json, "salt");
    cJSON *nonce_item = cJSON_GetObjectItemCaseSensitive(package_json, "nonce");
    cJSON *ciphertext_item = cJSON_GetObjectItemCaseSensitive(package_json, "ciphertext");
    cJSON *tag_item = cJSON_GetObjectItemCaseSensitive(package_json, "authentication_tag");

    if (!cJSON_IsString(salt_item) || !cJSON_IsString(nonce_item) || 
        !cJSON_IsString(ciphertext_item) || !cJSON_IsString(tag_item)) {
        cJSON_Delete(package_json);
        return -3;
    }

    size_t salt_len = 0;
    size_t nonce_len = 0;
    size_t ciphertext_len = 0;
    size_t tag_len = 0;

    unsigned char *salt = base64_decode(salt_item->valuestring, &salt_len);
    unsigned char *nonce = base64_decode(nonce_item->valuestring, &nonce_len);
    unsigned char *ciphertext = base64_decode(ciphertext_item->valuestring, &ciphertext_len);
    unsigned char *tag = base64_decode(tag_item->valuestring, &tag_len);

    cJSON_Delete(package_json);

    if (!salt || !nonce || !ciphertext || !tag) {
        free(salt);
        free(nonce);
        free(ciphertext);
        free(tag);
        return -4;
    }

    if (salt_len != 16 || nonce_len != 12 || tag_len != 16) {
        free(salt);
        free(nonce);
        free(ciphertext);
        free(tag);
        return -5;
    }

    unsigned char key[32];
    if (!pbkdf2_derive_key(password, salt, (int)salt_len, 100000, key, sizeof(key))) {
        free(salt);
        free(nonce);
        free(ciphertext);
        free(tag);
        return -6;
    }

    free(salt); 

    char *decrypted = aes_gcm_decrypt(ciphertext, (int)ciphertext_len, NULL, 0, tag, key, nonce, (int)nonce_len);
    
    OPENSSL_cleanse(key, sizeof(key));

    free(nonce);
    free(ciphertext);
    free(tag);

    if (!decrypted) {
        return -7; 
    }

    cJSON *payload = cJSON_Parse(decrypted);
    free(decrypted);

    if (!payload) {
        return -8;
    }

    cJSON *text_item = cJSON_GetObjectItemCaseSensitive(payload, "text");
    cJSON *recipient_type_item = cJSON_GetObjectItemCaseSensitive(payload, "recipient_type");
    cJSON *recipient_item = cJSON_GetObjectItemCaseSensitive(payload, "recipient");

    if (!cJSON_IsString(text_item) || !cJSON_IsString(recipient_type_item) || !cJSON_IsString(recipient_item)) {
        cJSON_Delete(payload);
        return -9;
    }

    *plaintext = strdup(text_item->valuestring);
    *recipient_type = strdup(recipient_type_item->valuestring);
    *recipient = strdup(recipient_item->valuestring);

    cJSON_Delete(payload);

    if (!*plaintext || !*recipient_type || !*recipient) {
        free(*plaintext);
        free(*recipient_type);
        free(*recipient);
        *plaintext = NULL;
        *recipient_type = NULL;
        *recipient = NULL;
        return -10;
    }

    return 0;
}