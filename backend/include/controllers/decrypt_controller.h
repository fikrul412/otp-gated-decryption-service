#ifndef DECRYPT_CONTROLLER_H
#define DECRYPT_CONTROLLER_H

#include "mongoose.h"

void decrypt_controller(
    struct mg_connection *c,
    struct mg_http_message *hm
);

#endif