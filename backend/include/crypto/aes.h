#ifndef AES_CRYPTO_H
#define AES_CRYPTO_H

#include <stddef.h>

// Configuration constants for AES-256
#define AES_KEY_SIZE 32 // 256-bit keys
#define AES_IV_SIZE  16 // 128-bit block size (required by AES)

/**
 * @brief Encrypts a null-terminated string using AES-256-CBC.
 * 
 * @param plaintext The null-terminated C-string to encrypt.
 * @param key The 32-byte binary encryption key.
 * @param iv The 16-byte binary initialization vector.
 * @return char* A dynamically allocated buffer containing the raw binary ciphertext, 
 *               or NULL on failure. Must be manually free()'d.
 */
char* aes_encrypt_cbc(const char *plaintext, const unsigned char *key, const unsigned char *iv);

/**
 * @brief Decrypts a binary ciphertext blob using AES-256-CBC.
 * 
 * @param ciphertext The raw binary buffer containing the encrypted data.
 * @param ciphertext_len The exact size of the ciphertext buffer in bytes.
 * @param key The 32-byte binary decryption key.
 * @param iv The 16-byte binary initialization vector.
 * @return char* A dynamically allocated, null-terminated C-string containing the decrypted 
 *               plaintext, or NULL on failure. Must be manually free()'d.
 */
char* aes_decrypt_cbc(const char *ciphertext, int ciphertext_len, const unsigned char *key, const unsigned char *iv);

#endif // AES_CRYPTO_H