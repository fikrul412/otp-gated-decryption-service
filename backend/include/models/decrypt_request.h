#ifndef DECRYPT_REQUEST_H
#define DECRYPT_REQUEST_H

typedef struct
{
    char *method;
    char *encrypted_data;
    char *otp;
    char *encryption_key;
} DecryptRequest;

#endif