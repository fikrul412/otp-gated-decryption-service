#include "crypto/aes.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>

int pbkdf2_derive_key(const char *password,
                     const unsigned char *salt,
                     int salt_len,
                     int iterations,
                     unsigned char *out_key,
                     int key_len)
{
    if (!password || !salt || salt_len <= 0 || !out_key || key_len <= 0) {
        return 0;
    }

    return PKCS5_PBKDF2_HMAC(password,
                             (int)strlen(password),
                             salt,
                             salt_len,
                             iterations,
                             EVP_sha256(),
                             key_len,
                             out_key);
}

char* aes_gcm_decrypt(const unsigned char *ciphertext,
                      int ciphertext_len,
                      const unsigned char *aad,
                      int aad_len,
                      const unsigned char *tag,
                      const unsigned char *key,
                      const unsigned char *iv,
                      int iv_len)
{
    if (!ciphertext || ciphertext_len <= 0 || !tag || !key || !iv || iv_len <= 0) {
        return NULL;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return NULL;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) goto error;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL) != 1) goto error;
    if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) != 1) goto error;

    int len = 0;
    if (aad && aad_len > 0) {
        if (EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len) != 1) goto error;
    }

    char *plaintext = malloc(ciphertext_len + 1);
    if (!plaintext) goto error;

    if (EVP_DecryptUpdate(ctx, (unsigned char*)plaintext, &len, ciphertext, ciphertext_len) != 1) goto error;
    int plaintext_len = len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)tag) != 1) goto error;
    if (EVP_DecryptFinal_ex(ctx, (unsigned char*)plaintext + len, &len) != 1) goto error;
    plaintext_len += len;
    plaintext[plaintext_len] = '\0';

    EVP_CIPHER_CTX_free(ctx);
    return plaintext;

error:
    free(plaintext);
    EVP_CIPHER_CTX_free(ctx);
    return NULL;
}

char* aes_gcm_encrypt(const unsigned char *plaintext,
                      int plaintext_len,
                      const unsigned char *aad,
                      int aad_len,
                      const unsigned char *key,
                      const unsigned char *iv,
                      int iv_len,
                      unsigned char *tag)
{
    if (!plaintext || plaintext_len < 0 || !key || !iv || iv_len <= 0 || !tag) {
        return NULL;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return NULL;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) goto error;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL) != 1) goto error;
    if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) != 1) goto error;

    if (aad && aad_len > 0) {
        int len = 0;
        if (EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len) != 1) goto error;
    }

    char *ciphertext = malloc(plaintext_len);
    if (!ciphertext) goto error;

    int len = 0;
    if (plaintext_len > 0) {
        if (EVP_EncryptUpdate(ctx, (unsigned char*)ciphertext, &len, plaintext, plaintext_len) != 1) goto error;
    }
    int ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, (unsigned char*)ciphertext + len, &len) != 1) goto error;
    ciphertext_len += len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) goto error;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;

error:
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    return NULL;
}