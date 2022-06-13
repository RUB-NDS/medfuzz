/**
* Network/Events/Connection.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENT_CONNECTION_H
#define NETWORK_EVENT_CONNECTION_H

#include <memory>

class NetworkSocket;

namespace Network {
    namespace Events {

class Connection
{
    public:
        Connection(std::shared_ptr<NetworkSocket> connection);
        Connection() = delete;
        virtual ~Connection();
        std::shared_ptr<NetworkSocket> connection();

    private:
        std::shared_ptr<NetworkSocket> m_connection;
};

}}

#endif //NETWORK_EVENT_CONNECTION_H
