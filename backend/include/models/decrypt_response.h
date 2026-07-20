#ifndef DECRYPT_RESPONSE_H
#define DECRYPT_RESPONSE_H

typedef struct
{
    char *message;
    char *recipient_type;
    char *recipient;
} DecryptResponse;

#endif