/**
* Network/Events/ClientConnection.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENTS_CLIENT_CONNECTION_H
#define NETWORK_EVENTS_CLIENT_CONNECTION_H

#include <memory>

class PeerSocket;
class ServerSocket;

namespace Network {
    namespace Events {

class ClientConnection
{
    public:
        ClientConnection(std::shared_ptr<ServerSocket> server,
            std::shared_ptr<PeerSocket> client);
        ClientConnection() = delete;
        virtual ~ClientConnection();
        std::shared_ptr<ServerSocket> server();
        std::shared_ptr<PeerSocket> client();

    private:
        std::shared_ptr<ServerSocket> m_server;
        std::shared_ptr<PeerSocket> m_client;
};

}}

#endif //NETWORK_EVENTS_CLIENT_CONNECTION_H
