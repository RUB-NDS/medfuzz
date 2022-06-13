/**
* Network/Events/Connection.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "Connection.h"

namespace Network {
    namespace Events {
Connection::Connection(std::shared_ptr<NetworkSocket> connection)
    : m_connection { connection }
{}

Connection::~Connection()
{}

std::shared_ptr<NetworkSocket> Connection::connection()
{
    return m_connection;
}

}}
