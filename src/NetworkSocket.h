/**
* NetworkSocket.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <poll.h>
#include <cstddef>
#include <thread>
#include "types.h"
#include "Closable.h"

class NetworkSocket : public virtual Closable
{
    public:
        virtual ~NetworkSocket() { }
        virtual void close() override;
        bool is_closed() const { return m_closed.load(std::memory_order_relaxed); }
        uint16_t get_port() const { return m_port; }
        void set_port(uint16_t port) { m_port = port; }
        void wait_exit();

    protected:
        explicit NetworkSocket(int hnd);
        explicit NetworkSocket(uint16_t port);
        NetworkSocket(const NetworkSocket&) = delete;

        void set_closed(bool closed) { m_closed.store(closed); }
        std::mutex& mtx() { return m_mtx_connection; }
        std::condition_variable& exit_condition() { return m_exit; }
        int& handle() { return m_handle; }
        std::thread& connection_worker() { return m_connection_worker; }
        void detach();
        int poll();
        pollfd& poll_fd() { return m_poll[0]; }
        bool has_poll_error();
        bool has_poll_data();
        bool poll_is_closed();
        void log_poll_errors();
        static uint16_t get_port(int sock_handle);

    private:
        int m_handle;
        std::array<pollfd, 1> m_poll;
        int m_port;
        std::thread m_connection_worker;
        std::atomic<bool> m_closed;
        std::mutex m_mtx_connection;
        std::condition_variable m_exit;
        static constexpr size_t POLLING_INTERVAL = 500;
};

#endif //NETWORK_SOCKET_H

