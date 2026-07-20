#ifndef OTP_REQUEST_H
#define OTP_REQUEST_H

typedef struct
{
    char *recipient_type;
    char *recipient;
} OtpRequest;

#endif