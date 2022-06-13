/**
* ServerSocket.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <algorithm>
#include <condition_variable>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <stdexcept>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <vector>
#include "Log.h"
#include "Network/Events/ClientConnected.h"
#include "Network/Events/ClientDisconnected.h"
#include "Network/Events/DataReceived.h"
#include "Network/Events/DataSent.h"
#include "PeerSocket.h"
#include "ServerSocket.h"

ServerSocket::ServerSocket (uint16_t port)
    : NetworkSocket (port),
      m_client_connected_evp(),
      m_client_disconnected_evp(),
      m_connection_closed_evp(),
      m_addr{ AF_INET, htons(port), { INADDR_ANY }, { 0 } }
{
}

std::shared_ptr<ServerSocket> ServerSocket::create(uint16_t port)
{
    auto sc = std::shared_ptr<ServerSocket>(new ServerSocket (port));
    sc->m_self = sc;
    return sc;
}

void ServerSocket::close()
{
    NetworkSocket::close();
    m_connection_closed_evp.notify({ m_self });
    exit_condition().notify_all();
    m_spawn_worker_cv.notify_all();
}

void ServerSocket::listen()
{
    std::unique_lock<std::mutex> lock { mtx() };
    if(!is_closed())
    {
        Log::warning("Ignored listen() call on active server connection.", Log::Verbosity::VERBOSE);
        exit_condition().notify_all();
        lock.unlock();
        return;
    }

    set_closed(false);
    bind(handle(), address(), sizeof(*address()));
    ::listen(handle(), MAX_PENDING_CONNECTIONS);
    fcntl(handle(), F_SETFL, O_NONBLOCK);
    set_port(get_port(handle()));
    exit_condition().notify_all();
    lock.unlock();

    try
    {
        connection_worker() = std::thread { [&] ()
            {
                std::vector<std::thread> client_workers {};
                std::mutex mtx_cc;
                bool waiting_for_client = false;
                std::unique_lock<std::mutex> srv_lock { mtx_cc };

                while(!is_closed())
                {
                    m_spawn_worker_cv.wait(srv_lock, [&]{ return !waiting_for_client || is_closed(); });
                    waiting_for_client = true;
                    client_workers.emplace_back(std::thread { [&] ()
                        {
                            try
                            {
                                sockaddr c_addr;
                                socklen_t c_len = sizeof(struct sockaddr);
                                    int p;
                                do
                                {
                                    p = poll();
                                }
                                while(p == 0 && !is_closed());

                                if(has_poll_error())
                                {
                                    log_poll_errors();
                                    waiting_for_client = false;
                                    if(srv_lock.owns_lock())
                                    {
                                        srv_lock.unlock();
                                    }
                                    m_spawn_worker_cv.notify_all();
                                    exit_condition().notify_all();
                                }
                                else
                                {
                                    int h = accept(handle(), &c_addr,
                                        static_cast<socklen_t*>(&c_len));

                                    if(h < 0)
                                    {
                                        if(errno == EINTR)
                                        {
                                            Log::info("Client worker received signal, exiting.", Log::Verbosity::DEBUG);
                                        }
                                        waiting_for_client = false;
                                        srv_lock.unlock();
                                        m_spawn_worker_cv.notify_all();
                                        exit_condition().notify_all();
                                        Log::error("Could not accept client connection: "
                                            + std::string(strerror(errno)),
                                            Log::Verbosity::NORMAL);
                                        return;
                                    }

                                    auto cc = PeerSocket::accept(h);
                                    auto ceh = std::make_shared<CloseEventHandler>(cc);
                                    closed_event().subscribe(ceh);
                                    cc->data_received_event().subscribe(m_self);
                                    cc->data_sent_event().subscribe(m_self);
                                    cc->open();
                                    waiting_for_client = false;
                                    m_client_connected_evp.notify({ m_self, cc });
                                    m_spawn_worker_cv.notify_all();
                                    srv_lock.unlock();
                                    Log::info("Server: Client connected.", Log::Verbosity::VERBOSE);
                                    cc->wait_exit();
                                    m_client_disconnected_evp.notify({ m_self, cc });
                                    m_spawn_worker_cv.notify_all();
                                    exit_condition().notify_all();
                                }
                            }
                            catch (const std::runtime_error& ex)
                            {
                                Log::error("Connection to client lost: " + std::string(ex.what()), Log::Verbosity::VERBOSE);
                                waiting_for_client = false;
                                if(srv_lock.owns_lock())
                                {
                                    srv_lock.unlock();
                                }
                                m_spawn_worker_cv.notify_all();
                                exit_condition().notify_all();
                            }
                        }
                    });
                }
                Log::info("Exiting server listener thread", Log::Verbosity::DEBUG);
                close();
                for(auto& c : client_workers)
                {
                    c.join();
                    exit_condition().notify_all();
                    m_spawn_worker_cv.notify_all();
                }
            }};
    }
    catch(const std::runtime_error& ex)
    {
        Log::error("Server error: " + std::string(ex.what()), Log::Verbosity::NORMAL);
        close();
    }
}

IEventProvider<Network::Events::ClientConnected>& ServerSocket::client_connected_event()
{
    return m_client_connected_evp;
}

IEventProvider<Network::Events::ClientDisconnected>& ServerSocket::client_disconnected_event()
{
    return m_client_disconnected_evp;
}

IEventProvider<Network::Events::ConnectionClosed>& ServerSocket::closed_event()
{
    return m_connection_closed_evp;
}

void ServerSocket::handle_event(Network::Events::DataReceived ev)
{
   m_data_received_evp.notify(ev);
}

void ServerSocket::handle_event(Network::Events::DataSent ev)
{
    m_data_sent_evp.notify(ev);
}

sockaddr* ServerSocket::address()
{
    return (sockaddr*)&m_addr;
}

