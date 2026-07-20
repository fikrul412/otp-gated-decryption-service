#ifndef OTP_SERVICE_H
#define OTP_SERVICE_H

#include "models/otp_request.h"
#include "models/otp_response.h"

OtpResponse otp_service(
    const OtpRequest *request
);

#endif