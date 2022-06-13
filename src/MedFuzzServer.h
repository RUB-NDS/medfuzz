/**
* MedFuzzServer.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef MED_FUZZ_SERVER_H
#define MED_FUZZ_SERVER_H

#include <memory>
#include <stdint.h>
#include "EventListener.h"
#include "FuzzingEventHooks.h"

class ServerSocket;
namespace Network { namespace Events { class ClientConnected; } }

class MedFuzzServer final
    : public EventListener<Network::Events::ClientConnected>,
      public FuzzingEventHooks
{
    public:
        ~MedFuzzServer();
        void start();
        void handle_event(Network::Events::ClientConnected ev) override;
        static std::shared_ptr<MedFuzzServer> create(uint16_t server_port);

    private:
        explicit MedFuzzServer(uint16_t server_port);
        std::shared_ptr<ServerSocket> m_connection;
        std::shared_ptr<MedFuzzServer> m_self;
};

#endif //MED_FUZZ_SERVER_H
