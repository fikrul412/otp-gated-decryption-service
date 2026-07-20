#ifndef HEALTH_CONTROLLER_H
#define HEALTH_CONTROLLER_H

#include "mongoose.h"

void health_controller(
    struct mg_connection *c,
    struct mg_http_message *hm
);

#endif