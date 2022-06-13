/**
* Network/Events/ClientConnection.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "ClientConnection.h"

namespace Network {
    namespace Events {

ClientConnection::ClientConnection(std::shared_ptr<ServerSocket> server,
    std::shared_ptr<PeerSocket> client)
    : m_server { server }, m_client { client }
{}

ClientConnection::~ClientConnection()
{}

std::shared_ptr<ServerSocket> ClientConnection::server()
{
    return m_server;
}

std::shared_ptr<PeerSocket> ClientConnection::client()
{
    return m_client;
}

}}
