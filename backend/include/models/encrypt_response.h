#ifndef ENCRYPT_RESPONSE_H
#define ENCRYPT_RESPONSE_H

typedef struct
{
    char *encrypted_message;
    char *generated_key;
    char *recipient_type;
    char *recipient;
} EncryptResponse;

#endif