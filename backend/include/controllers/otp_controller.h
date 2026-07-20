#ifndef OTP_CONTROLLER_H
#define OTP_CONTROLLER_H

#include "mongoose.h"

void otp_controller(
    struct mg_connection *c,
    struct mg_http_message *hm
);

#endif