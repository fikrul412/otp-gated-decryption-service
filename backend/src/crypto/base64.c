#include "crypto/base64.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <string.h>
#include <stdlib.h>

char* base64_encode(const unsigned char *src, size_t len) {
    if (!src || len == 0) return NULL;

    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 
    
    BIO_write(bio, src, (int)len);
    (void)BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    char *encoded = malloc(bufferPtr->length + 1);
    if (encoded) {
        memcpy(encoded, bufferPtr->data, bufferPtr->length);
        encoded[bufferPtr->length] = '\0';
    }

    BIO_free_all(bio);
    return encoded;
}

unsigned char* base64_decode(const char *src, size_t *out_len) {
    if (!src || !out_len) return NULL;

    size_t len = strlen(src);
    if (len == 0) return NULL;

    unsigned char *decoded = malloc(len);
    if (!decoded) return NULL;

    BIO *bio, *b64;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(src, (int)len);
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int decoded_len = BIO_read(bio, decoded, (int)len);

    if (decoded_len < 0) {
        free(decoded);
        BIO_free_all(bio);
        return NULL;
    }

    *out_len = (size_t)decoded_len;
    BIO_free_all(bio);
    return decoded;
}