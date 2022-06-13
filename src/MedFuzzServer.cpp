/**
* MedFuzzServer.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "EventProvider.h"
#include "MedFuzzServer.h"
#include "Log.h"
#include "Network/Events/ClientConnected.h"
#include "ServerSocket.h"

MedFuzzServer::MedFuzzServer(uint16_t server_port)
    : m_connection( ServerSocket::create(server_port)),
      m_self(nullptr)
{}

MedFuzzServer::~MedFuzzServer()
{
    m_connection->client_connected_event().unsubscribe(m_self);
}

std::shared_ptr<MedFuzzServer> MedFuzzServer::create(uint16_t server_port)
{
    auto instance = std::shared_ptr<MedFuzzServer>(new MedFuzzServer(server_port));
    instance->m_self = instance;
    return instance;
}

void MedFuzzServer::start()
{
    if(!m_connection->is_closed())
    {
        Log::warning("Ignoring start()-call on established server connection.", Log::Verbosity::DEBUG);
        return;
    }
    m_connection->client_connected_event().subscribe(m_self);
    m_connection->listen();
    m_connection->wait_exit();
}

void MedFuzzServer::handle_event(Network::Events::ClientConnected ev)
{
    Log::info("Processing client init events.", Log::Verbosity::TRACE);
    setup_io_events(ev.client());
    client_init(ev.client());
    Log::info("Done processing client init events.", Log::Verbosity::TRACE);
}


