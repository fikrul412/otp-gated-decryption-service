#ifndef OTP_RESPONSE_H
#define OTP_RESPONSE_H

typedef struct
{
    int success;
} OtpResponse;

OtpResponse otp_response_success(void);
OtpResponse otp_response_failure(void);

#endif