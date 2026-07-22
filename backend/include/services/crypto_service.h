#ifndef CRYPTO_SERVICE_H
#define CRYPTO_SERVICE_H

/**
 * @brief Decrypts a full AES-GCM encrypted package using a password-derived key.
 * 
 * @param password The user-supplied password.
 * @param encrypted_package JSON string containing salt, nonce, ciphertext, and authentication_tag.
 * @param plaintext Receives a newly allocated plaintext string on success.
 * @param recipient_type Receives a newly allocated recipient_type string on success.
 * @param recipient Receives a newly allocated recipient string on success.
 * @return int 0 on success, non-zero on failure.
 */
int decrypt_package(
    const char *password,
    const char *encrypted_package,
    char **plaintext,
    char **recipient_type,
    char **recipient
);

#endif // CRYPTO_SERVICE_H