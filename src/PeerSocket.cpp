/**
* PeerSocket.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <algorithm>
#include <arpa/inet.h>
#include <condition_variable>
#include <cstdint>
#include <errno.h>
#include <fcntl.h>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include "Network/Events/ConnectionFailed.h"
#include "types.h"
#include "literals.h"
#include "PeerSocket.h"
#include "Network/Events/DataReceived.h"
#include "Network/Events/DataSent.h"
#include "Log.h"

PeerSocket::PeerSocket(int handle)
    : NetworkSocket (handle),
      m_send_counter(0)
{
    Log::info("Creating client connection to host: " + hostname() + " on port: " + std::to_string(this->get_port()), Log::Verbosity::TRACE);
}

PeerSocket::PeerSocket(const std::string& hostname, uint16_t port)
    : NetworkSocket (port),
      m_hostname(hostname),
      m_send_counter(0)

{
    Log::info("Creating client connection to host: " + hostname + " on port: " + std::to_string(port), Log::Verbosity::TRACE);
}

std::shared_ptr<PeerSocket> PeerSocket::create(std::string service_hostname, uint16_t service_port)
{
    auto pc = std::shared_ptr<PeerSocket>(new PeerSocket (service_hostname, service_port));
    pc->m_self = pc;
    return pc;
}

std::shared_ptr<PeerSocket> PeerSocket::accept(int handle)
{
    auto pc = std::shared_ptr<PeerSocket>(new PeerSocket (handle));
    pc->m_self = pc;
    return pc;
}

void PeerSocket::open()
{
    establish();
    listen();
}

void PeerSocket::establish()
{
    std::unique_lock<std::mutex> lock { mtx() };
    if(is_closed())
    {
        hostent_t* m_srv = gethostbyname(hostname().c_str());
        if(m_srv == nullptr)
        {
             Log::error("Unable to resolve hostname: " + std::string(strerror(errno)), Log::Verbosity::NORMAL);
        }

        handle() = socket(AF_INET, SOCK_STREAM, 0);
        fcntl(handle(), F_SETFL, O_NONBLOCK);
        inetaddr_t s_addr { AF_INET, htons( get_port()), { 0 }, { 0 } };
        inet_aton(m_srv->h_addr_list[0], &(s_addr.sin_addr));
        lock.unlock();
        int s = connect(handle(), (sockaddr_t*)&s_addr, sizeof(s_addr));
        lock.lock();
        if(s < 0)
        {
            if(errno == EINPROGRESS)
            {
                fd_set wfs;
                timeval timeout { 3, 0 };
                FD_ZERO(&wfs);
                FD_SET(handle(), &wfs);
                s = select(handle() + 1, NULL, &wfs, NULL, &timeout);
            }

            if(s < 1)
            {
                Log::error("Server connection failed: " + std::string(strerror(errno)),
                           Log::Verbosity::NORMAL);
                lock.unlock();
                m_connection_failed_evp.notify({m_self});
                exit_condition().notify_all();
                return;
            }
        }
        poll_fd() = { handle(), POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL, 0 };
        set_closed(false);
        Log::info("Client connected to " + hostname() + " on port " + std::to_string( get_port()), Log::Verbosity::TRACE);
    }
    lock.unlock();
}

void PeerSocket::listen()
{
    if(connection_worker().joinable())
    {
        Log::warning("Ignoring excess listen call on active socket.", Log::Verbosity::VERBOSE);
        return;
    }
    connection_worker() = std::thread { [&] ()
        {
            try
            {
                while(!is_closed())
                {
                    Log::info("Connection open, client connection worker polling for incomding data.", Log::Verbosity::TRACE);
                    read(handle());
                    Log::info("Client connection worker checking connection status.", Log::Verbosity::TRACE);
                }
            }
            catch (const std::runtime_error& ex)
            {
                Log::error("Error reading client connection: " + std::string(ex.what()), Log::Verbosity::NORMAL);
                set_closed(true);
                m_connection_failed_evp.notify({m_self});
            }
            Log::info("Exiting client connection worker for handle \"" + std::to_string(handle()) + "\".", Log::Verbosity::DEBUG);
            exit_condition().notify_all();
        }
    };
}

void PeerSocket::close()
{
    NetworkSocket::close();
}

void PeerSocket::write(std::vector<uint8_t> data)
{
    Log::info("Writing data: " + std::string(data.begin(), data.end()), Log::Verbosity::TRACE);
    std::unique_lock<std::mutex> lock { mtx() };
    if(::write(handle(), data.data(), data.size()) < 0)
    {
        Log::error("Socket write failed: " + std::string(strerror(errno)), Log::Verbosity::NORMAL);
    }
    m_send_counter++;
    Log::info(std::to_string(data.size()) + " bytes written", Log::Verbosity::TRACE);
    m_data_sent_evp.notify({ m_self, data });
    lock.unlock();
    exit_condition().notify_all();
}

void PeerSocket::write(std::string data)
{
    std::unique_lock<std::mutex> lock { mtx() };
    if(::write(handle(), data.data(), data.size()) < 0)
    {
        Log::error("Socket write failed: " + std::string(strerror(errno)), Log::Verbosity::NORMAL);
    }
    else
    {
        Log::info("Writing data: " + data, Log::Verbosity::TRACE);
    }
    m_send_counter++;
    Log::info(std::to_string(data.size()) + " bytes written", Log::Verbosity::TRACE);
    m_data_sent_evp.notify({ m_self, std::vector<uint8_t>(data.begin(), data.end()) });
    lock.unlock();
    exit_condition().notify_all();
}

size_t PeerSocket::read(int handle)
{
    std::vector<uint8_t> buf(BUFFER_SIZE);
    size_t read_total = 0;
    int r;

    std::unique_lock<std::mutex> lock { mtx() };
    if(poll() == 0)
    {
        Log::info("Poll", Log::Verbosity::TRACE);
        return 0;
    }

    if(has_poll_error())
    {
        if(!is_closed())
        {
            log_poll_errors();
            close();
            exit_condition().notify_all();
        }
        return 0;
    }

    while(has_poll_data() && (r = ::read(handle, buf.data(), buf.size())) > 0)
    {
        lock.unlock();
        m_data_received_evp.notify({ m_self, std::vector<uint8_t>(
            buf.begin(), buf.begin() + r) });
        lock.lock();
        read_total += std::max(static_cast<size_t>(r), 0_z);
    }
    lock.unlock();
    if(r < 0)
    {
        const std::string err = "Peer connection error: " + std::string(strerror(errno));
        Log::error(err, Log::Verbosity::NORMAL);
        m_connection_failed_evp.notify({m_self});
        close();
    }

    return std::max(read_total, 0_z);
}

IEventProvider<Network::Events::ConnectionClosed>&
PeerSocket::connection_closed_event()
{
    return m_connection_closed_evp;
}

IEventProvider<Network::Events::ConnectionFailed>&
PeerSocket::connection_failed_event()
{
    return m_connection_failed_evp;
}
