#ifndef ROUTER_H
#define ROUTER_H

#include "mongoose.h"

void router(
    struct mg_connection *c,
    struct mg_http_message *hm
);

#endif