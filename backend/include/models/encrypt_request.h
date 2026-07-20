#ifndef ENCRYPT_REQUEST_H
#define ENCRYPT_REQUEST_H

typedef struct
{
    char *method;
    char *message;
    char *recipient_type;
    char *recipient;
} EncryptRequest;

#endif