/**
* PeerSocket.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef PEER_SOCKET_H
#define PEER_SOCKET_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "EventProvider.h"
#include "NetworkSocket.h"
#include "Network/IO.h"

namespace Network { namespace Events {
    class ConnectionClosed;
    class ConnectionFailed;
}}

class PeerSocket
    : public Network::IO, public NetworkSocket
{
    public:
        virtual void write(std::vector<uint8_t> data) override;
        virtual void write(std::string data) override;
        const std::string& hostname() const { return m_hostname ;}
        uint64_t get_send_message_count() const { return m_send_counter; };
        void open();
        void close() override;
        IEventProvider<Network::Events::ConnectionFailed>& connection_failed_event();
        IEventProvider<Network::Events::ConnectionClosed>& connection_closed_event();

        static std::shared_ptr<PeerSocket> create(
            std::string service_hostname, uint16_t service_port);
        static std::shared_ptr<PeerSocket> accept(int handle);

    private:
        explicit PeerSocket(int handle);
        PeerSocket(const std::string& hostname, uint16_t port);

        size_t read(int handle);
        void listen();
        void establish();
        std::string m_hostname;
        uint64_t m_send_counter;
        std::shared_ptr<PeerSocket> m_self;
        EventProvider<Network::Events::ConnectionFailed>
        m_connection_failed_evp;
        EventProvider<Network::Events::ConnectionClosed>
        m_connection_closed_evp;
        static constexpr size_t BUFFER_SIZE = 8192;
};

#endif //PEER_SOCKET_H
