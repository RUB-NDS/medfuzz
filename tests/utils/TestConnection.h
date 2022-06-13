/**
* utils/TestConnection.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TEST_UTILS_TEST_CONNECTION_H
#define TEST_UTILS_TEST_CONNECTION_H

#include <memory>
#include "ServerSocket.h"
#include "PeerSocket.h"

namespace Tests { namespace Utils {

class TestConnection
{
    public:
        TestConnection ( uint16_t server_port )
            : m_server ( ServerSocket::create ( server_port ) ),
              m_client ( PeerSocket::create ( "localhost", server_port )) {}

        void establish()
        {
            m_server->listen();
            if ( m_server->is_closed() ) {
                throw std::runtime_error ( "Server socket initialization failed." );
            }

            m_client->open();
            if ( m_client->is_closed() ) {
                throw std::runtime_error ( "Client PeerConnection failed." );
            }
        }

        std::shared_ptr<ServerSocket> server()
        {
            return m_server;
        }

        std::shared_ptr<PeerSocket>  client()
        {
            return m_client;
        }

        private:
        std::shared_ptr<ServerSocket> m_server;
        std::shared_ptr<PeerSocket> m_client;
};
}
}

#endif //TEST_UTILS_TEST_CONNECTION_H
