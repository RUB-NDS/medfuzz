/**
* types.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct sockaddr_in inetaddr_t;
typedef struct sockaddr sockaddr_t;
typedef struct hostent hostent_t;

#endif //TYPES_H

