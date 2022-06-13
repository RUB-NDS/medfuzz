/**
* NetworkSocket.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <bits/exception.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "Log.h"
#include "types.h"
#include "NetworkSocket.h"

NetworkSocket::NetworkSocket (uint16_t port)
    : m_handle(socket(AF_INET, SOCK_STREAM, 0)),
      m_poll( { handle(), POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL, 0 } ),
      m_port(port),
      m_connection_worker(),
      m_closed(true),
      m_mtx_connection()
{
}


NetworkSocket::NetworkSocket (int hnd)
    : m_handle(hnd),
      m_poll( { handle(), POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL, 0 } ),
      m_port(get_port(handle())),
      m_connection_worker(),
      m_closed(false),
      m_mtx_connection()
{
}

void NetworkSocket::close()
{
    if(!m_closed.exchange(true))
    {
       ::close(handle());
    }

    m_exit.notify_all();
}

void NetworkSocket::detach()
{
    std::lock_guard<std::mutex> lock { mtx() };
    if(!m_connection_worker.joinable())
        return;

    m_connection_worker.detach();
}

int NetworkSocket::poll()
{
    int p = ::poll(m_poll.data(), m_poll.size(), POLLING_INTERVAL);
    Log::info("NetworkSocket::poll result: " + std::to_string(m_poll[0].revents),
              Log::Verbosity::TRACE);
    return p;
}

void NetworkSocket::wait_exit()
{
    std::unique_lock<std::mutex> lock { mtx() };
    m_exit.wait(lock, [&] { return is_closed(); });
    try
    {
        if(m_connection_worker.joinable())
        {
            lock.unlock();
            m_exit.notify_all();
            m_connection_worker.join();
            return;
        }
    }
    catch (std::exception& e)
    {
        Log::error("Error while waiting for network connection thread to exit.",
                   Log::Verbosity::NORMAL);
    }
    m_exit.notify_all();
}

uint16_t NetworkSocket::get_port(int sock_handle)
{
    inetaddr_t inet_addr;
    socklen_t s_len;
    s_len = sizeof(in_addr);
    if(getsockname(sock_handle, (sockaddr*)&inet_addr, static_cast<socklen_t*>(&s_len)) < 0)
    {
        Log::warning("Unable to determine port number for socket: " + std::to_string(sock_handle) + ".", Log::Verbosity::DEBUG);
        return 0;
    }
    return ntohs(inet_addr.sin_port);
}

bool NetworkSocket::has_poll_error()
{
    return m_poll[0].revents & (POLLRDHUP | POLLERR | POLLHUP | POLLNVAL);
}

bool NetworkSocket::poll_is_closed()
{
    return m_poll[0].revents & (POLLNVAL | POLLERR);
}

void NetworkSocket::log_poll_errors()
{
    const short& revents { m_poll[0].revents };
    if(revents & POLLRDHUP)
    {
        Log::error("Connection closed by peer.", Log::Verbosity::NORMAL);
    }
    if(revents & POLLERR)
    {
        Log::error("Peer connection error: " + std::string(strerror(errno)), Log::Verbosity::NORMAL);
    }
    if(revents & POLLHUP)
    {
        Log::error("Connection closed.", Log::Verbosity::NORMAL);
    }
    if(revents & POLLNVAL)
    {
        Log::error("Socket not open.", Log::Verbosity::NORMAL);
    }
}

bool NetworkSocket::has_poll_data()
{
    return m_poll[0].revents & POLLIN;
}
