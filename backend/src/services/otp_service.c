#include "services/otp_service.h"

OtpResponse otp_service(
    const OtpRequest *request)
{
    (void) request;

    OtpResponse response;

    response.success = 1;

    return response;
}