/**
* Settings.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <utility>
#include <string>
#include "Log.h"

class Settings
{
    public:
        enum Modes
        {
            Client,
            Server
        };

        static std::map<std::string, Modes> get_modes_map()
        {
            return { { "client", Modes::Client }, { "server", Modes::Server } };
        }

        static std::map<std::string, Log::Verbosity> get_verbosity_map()
        {
            return
            {
                { "normal", Log::Verbosity::NORMAL },
                { "verbose", Log::Verbosity::VERBOSE },
                { "debug", Log::Verbosity::DEBUG },
                { "trace", Log::Verbosity::TRACE }
            };
        }

        static Settings& get()
        {
            static Settings self {};
            return self;
        }

        uint16_t get_control_port() const
        {
            return m_control_port;
        }

        void set_control_port(const uint16_t& port)
        {
            m_control_port = port;
        }

        const uint16_t& get_target_port() const
        {
            return m_target_port;
        }

        void set_target_port(const uint16_t& port)
        {
            m_target_port = port;
        }

        const std::string& get_response_data_file() const
        {
            return m_response_data_file;
        }

        void set_response_data_file(const std::string& file)
        {
            m_response_data_file = file;
        }

        const std::string& get_initial_data_file() const
        {
            return m_initial_data_file;
        }

        void set_initial_data_file(const std::string& file)
        {
            m_initial_data_file = file;
        }

        const std::string& get_host_name() const
        {
            return m_host_name;
        }

        void set_host_name(const std::string& host_name)
        {
            m_host_name = host_name;
        }

        bool terminates_on_receive() const
        {
            return m_terminate_on_receive;
        }

        void set_terminate_on_receive(const int64_t& t)
        {
            m_terminate_on_receive = (t > 0);
        }

        void set_terminate_on_receive(bool t)
        {
            m_terminate_on_receive = t;
        }

        bool terminates_after_reply() const
        {
            return m_terminate_after_reply;
        }

        void set_terminate_after_reply(const int64_t& t)
        {
            m_terminate_after_reply = (t > 0);
        }

        void set_terminate_after_reply(bool t)
        {
            m_terminate_after_reply = t;
        }

        Modes get_mode() const
        {
            return m_mode;
        }

        void set_mode(Modes mode)
        {
            m_mode = mode;
        }

        Log::Verbosity get_verbosity() const
        {
            return m_verbosity;
        }

        void set_verbosity(Log::Verbosity verbosity)
        {
            m_verbosity = verbosity;
        }

        void set_echo_mode_enabled(const int64_t& echo_mode)
        {
            m_echo_mode = (echo_mode > 0);
        }

        void set_echo_mode_enabled(bool echo_mode)
        {
            m_echo_mode = echo_mode;
        }

        bool is_echo_mode_enabled() const
        {
            return m_echo_mode;
        }

    private:
        Settings()
           : m_control_port(12345),
             m_target_port(0), m_response_data_file(),
             m_initial_data_file(), m_host_name("localhost"),
             m_terminate_on_receive(false),
             m_terminate_after_reply(false),
             m_echo_mode(false), m_mode(Modes::Server),
             m_verbosity(Log::Verbosity::NORMAL) {}
        Settings(const Settings&) = delete;
        Settings(Settings &&) = delete;
        Settings& operator=(const Settings&) = delete;
        Settings& operator=(Settings&&) = delete;
        uint16_t m_control_port;
        uint16_t m_target_port;
        std::string m_response_data_file;
        std::string m_initial_data_file;
        std::string m_host_name;
        bool m_terminate_on_receive;
        bool m_terminate_after_reply;
        bool m_echo_mode;
        Modes m_mode;
        Log::Verbosity m_verbosity;
};

#endif //SETTINGS_H
