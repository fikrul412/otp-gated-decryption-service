#ifndef AES_CRYPTO_H
#define AES_CRYPTO_H

#include <stddef.h>

// Configuration constants for AES-256
#define AES_KEY_SIZE 32 // 256-bit keys
#define AES_IV_SIZE  16 // 128-bit block size (required by AES)

/**
 * Derives a 32-byte AES key from a password and salt using PBKDF2-SHA256.
 *
 * @param password User-supplied password.
 * @param salt Binary salt bytes.
 * @param salt_len Length of the salt in bytes.
 * @param iterations PBKDF2 iteration count.
 * @param out_key Buffer to write the derived key to. Must be at least key_len bytes.
 * @param key_len Desired key length in bytes.
 * @return int 1 on success, 0 on failure.
 */
int pbkdf2_derive_key(const char *password, const unsigned char *salt, int salt_len, int iterations, unsigned char *out_key, int key_len);

/**
 * Decrypts AES-256-GCM ciphertext.
 *
 * @param ciphertext Binary ciphertext bytes.
 * @param ciphertext_len Length of the ciphertext in bytes.
 * @param aad Additional authenticated data (optional).
 * @param aad_len Length of AAD.
 * @param tag 16-byte authentication tag.
 * @param key 32-byte AES key.
 * @param iv Initialization vector.
 * @param iv_len Length of IV in bytes.
 * @return char* Newly allocated plaintext string on success, or NULL on failure.
 */
char* aes_gcm_decrypt(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *aad, int aad_len, const unsigned char *tag, const unsigned char *key, const unsigned char *iv, int iv_len);

/**
 * Encrypts plaintext using AES-256-GCM.
 *
 * @param plaintext Raw plaintext bytes.
 * @param plaintext_len Length of the plaintext in bytes.
 * @param aad Additional authenticated data.
 * @param aad_len Length of the AAD.
 * @param key 32-byte AES key.
 * @param iv Initialization vector.
 * @param iv_len Length of IV in bytes.
 * @param tag Buffer to receive 16-byte authentication tag.
 * @return char* Newly allocated ciphertext buffer on success, or NULL on failure.
 */
char* aes_gcm_encrypt(const unsigned char *plaintext, int plaintext_len, const unsigned char *aad, int aad_len, const unsigned char *key, const unsigned char *iv, int iv_len, unsigned char *tag);

#endif // AES_CRYPTO_H