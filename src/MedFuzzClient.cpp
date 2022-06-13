/**
* MedFuzzClient.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "MedFuzzClient.h"
#include <bits/exception.h>
#include <stddef.h>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include "EventProvider.h"
#include "Log.h"
#include "Network/Events/DataReceived.h"
#include "PeerSocket.h"
#include "ServerSocket.h"
#include "Settings.h"
#include "StringTools.h"

MedFuzzClient::MedFuzzClient(uint16_t control_port)
    : m_control_server( ServerSocket::create(control_port)),
      m_connection(nullptr),
      m_self(nullptr)
{
}

MedFuzzClient::~MedFuzzClient()
{
    m_control_server->data_received_event().unsubscribe(m_self);
}

void MedFuzzClient::start()
{
    if(m_connection->is_closed())
    {
        Log::warning("Ignoring start()-call on established control server connection.", Log::Verbosity::DEBUG);
        return;
    }
    m_control_server->data_received_event().subscribe(m_self);
    m_control_server->listen();
    m_control_server->wait_exit();
}

void MedFuzzClient::handle_event(Network::Events::DataReceived ev)
{
    const std::string& host_name = Settings::get().get_host_name();
    uint16_t port = StringTools::from_string<uint16_t>({ ev.data().begin(), ev.data().end() });
    Log::info("Bind callback connecting to server: " + host_name + " on port: " + std::to_string(port), Log::Verbosity::TRACE);
    size_t wait = 1;
    size_t tries = 0;
    const size_t MAX_RETRIES = 10;
    while(++tries <= MAX_RETRIES)
    {
        try
        {
            m_connection = PeerSocket::create(Settings::get().get_host_name(), port);
            m_connection->open();
            break;
        }
        catch(std::exception& e)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait * (1 << tries)));
        }
        setup_io_events(m_connection);
    }
    client_init(m_connection);
}

std::shared_ptr<MedFuzzClient> MedFuzzClient::create(uint16_t control_port)
{
    auto instance = std::shared_ptr<MedFuzzClient>(new MedFuzzClient(control_port));
    instance->m_self = instance;
    return instance;
}


