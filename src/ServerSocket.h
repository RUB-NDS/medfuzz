/**
* ServerSocket.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>
#include <utility>
#include "types.h"
#include "PeerSocket.h"
#include "NetworkSocket.h"
#include "EventListener.h"
#include "EventProvider.h"
#include "Network/IOEventProvider.h"
#include "Network/Events/ConnectionClosed.h"

namespace Network { namespace Events {
    class ClientConnected;
    class ClientDisconnected;
    class DataReceived;
    class DataSent;
}}

class ServerSocket
    : public NetworkSocket,
      public EventListener<Network::Events::DataReceived>,
      public EventListener<Network::Events::DataSent>,
      public Network::IOEventProvider
{
    public:
        void listen();
        virtual void close() override;
        virtual void handle_event(Network::Events::DataReceived ev) override;
        virtual void handle_event(Network::Events::DataSent ev) override;
        IEventProvider<Network::Events::ClientConnected>& client_connected_event();
        IEventProvider<Network::Events::ClientDisconnected>& client_disconnected_event();
        IEventProvider<Network::Events::ConnectionClosed>& closed_event();
        static std::shared_ptr<ServerSocket> create(uint16_t port);

    private:
        class CloseEventHandler :
            public EventListener<Network::Events::ConnectionClosed>
        {
            public:
                explicit CloseEventHandler(std::shared_ptr<PeerSocket> connection)
                    : m_connection(connection) {}
                CloseEventHandler() = delete;
                virtual ~CloseEventHandler() {}
                CloseEventHandler(const CloseEventHandler&) = delete;

                void handle_event(Network::Events::ConnectionClosed ev)
                {
                    m_connection->close();
                }
            private:
                std::shared_ptr<PeerSocket> m_connection;
        };

        class ClientConnectionWorker
        {
            public:
                explicit ClientConnectionWorker(std::thread&& worker_thread)
                    : m_thread(std::move(worker_thread)), m_is_connected(false) {}
                ClientConnectionWorker(const std::thread&) = delete;
                ClientConnectionWorker() = delete;

                std::thread& thread() { return m_thread; }
                bool is_connected() { return m_is_connected; }
                void set_connected(bool is_connected) { m_is_connected = is_connected; }

            private:
                std::thread m_thread;
                bool m_is_connected;
        };

        explicit ServerSocket(uint16_t service_port);
        virtual sockaddr* address();
        EventProvider<Network::Events::ClientConnected> m_client_connected_evp;
        EventProvider<Network::Events::ClientDisconnected> m_client_disconnected_evp;
        EventProvider<Network::Events::ConnectionClosed> m_connection_closed_evp;
        inetaddr_t m_addr;
        std::condition_variable m_spawn_worker_cv;
        std::shared_ptr<ServerSocket> m_self;

        static constexpr size_t MAX_PENDING_CONNECTIONS = 2;
};

#endif //SERVER_SOCKET_H
