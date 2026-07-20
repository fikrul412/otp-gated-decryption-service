#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>

/**
 * @brief Encodes raw binary data into a Base64 null-terminated string.
 * 
 * @param src Pointer to the raw binary data bytes.
 * @param len The exact number of bytes to encode.
 * @return char* Dynamically allocated null-terminated Base64 string, or NULL on failure.
 *               Must be manually free()'d.
 */
char* base64_encode(const unsigned char *src, size_t len);

/**
 * @brief Decodes a Base64 string back into raw binary bytes.
 * 
 * @param src The null-terminated Base64 string to decode.
 * @param out_len Pointer to a size_t where the decoded binary byte count will be stored.
 * @return unsigned char* Dynamically allocated raw binary buffer, or NULL on failure.
 *                        Must be manually free()'d.
 */
unsigned char* base64_decode(const char *src, size_t *out_len);

#endif // BASE64_H