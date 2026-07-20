#ifndef ENCRYPT_CONTROLLER_H
#define ENCRYPT_CONTROLLER_H

#include "mongoose.h"

void encrypt_controller(
    struct mg_connection *c,
    struct mg_http_message *hm
);

#endif