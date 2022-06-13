/**
* Network/Events/ClientConnected.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENTS_CLIENT_CONNECTED_H
#define NETWORK_EVENTS_CLIENT_CONNECTED_H

#include "Network/Events/ClientConnection.h"

namespace Network {
    namespace Events {

class ClientConnected : public virtual ClientConnection
    { using ClientConnection::ClientConnection; };
}}

#endif //NETWORK_EVENTS_CLIENT_CONNECTED_H
