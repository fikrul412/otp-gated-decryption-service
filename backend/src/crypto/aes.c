#include "crypto/aes.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdlib.h>

char* aes_encrypt_cbc(const char *plaintext, const unsigned char *key, const unsigned char *iv) {
    if (!plaintext || !key || !iv) return NULL;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return NULL;

    int plaintext_len = (int)strlen(plaintext);
    // Max ciphertext size includes space for block padding
    int max_ciphertext_len = plaintext_len + AES_IV_SIZE;
    unsigned char *ciphertext = malloc(max_ciphertext_len);
    if (!ciphertext) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int len = 0;
    int ciphertext_len = 0;

    // Initialize encryption operation using AES-256-CBC
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) goto error;
    
    // Provide the message to be encrypted
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char*)plaintext, plaintext_len) != 1) goto error;
    ciphertext_len = len;

    // Finalize the encryption (handles PKCS7 padding)
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) goto error;
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    
    // NOTE: Because this outputs raw binary data, you'll want to pass 'ciphertext'
    // through a Base64 encoder before returning it to a JSON web client.
    return (char*)ciphertext;

error:
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    return NULL;
}

char* aes_decrypt_cbc(const char *ciphertext, int ciphertext_len, const unsigned char *key, const unsigned char *iv) {
    if (!ciphertext || ciphertext_len <= 0 || !key || !iv) return NULL;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return NULL;

    // Plaintext will be equal to or shorter than the ciphertext length
    char *plaintext = malloc(ciphertext_len + 1);
    if (!plaintext) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int len = 0;
    int plaintext_len = 0;

    // Initialize decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) goto error;

    // Provide the binary ciphertext to be decrypted
    if (EVP_DecryptUpdate(ctx, (unsigned char*)plaintext, &len, (const unsigned char*)ciphertext, ciphertext_len) != 1) goto error;
    plaintext_len = len;

    // Finalize decryption and strip the padding
    if (EVP_DecryptFinal_ex(ctx, (unsigned char*)plaintext + len, &len) != 1) goto error;
    plaintext_len += len;

    // Safely add null-terminator for string usage
    plaintext[plaintext_len] = '\0';

    EVP_CIPHER_CTX_free(ctx);
    return plaintext;

error:
    free(plaintext);
    EVP_CIPHER_CTX_free(ctx);
    return NULL;
}