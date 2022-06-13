/**
* Network/Events/ConnectionClosed.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENTS_CONNECTION_CLOSED_H
#define NETWORK_EVENTS_CONNECTION_CLOSED_H

#include "Network/Events/Connection.h"

namespace Network {
    namespace Events {

class ConnectionClosed : public virtual Connection
    { using Connection::Connection; };
}}

#endif //NETWORK_EVENTS_CONNECTION_CLOSED_H
