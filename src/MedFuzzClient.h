/**
* MedFuzzClient.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef MED_FUZZ_CLIENT_H
#define MED_FUZZ_CLIENT_H

#include <cstdint>
#include <memory>
#include "EventListener.h"
#include "FuzzingEventHooks.h"

class PeerSocket;
class ServerSocket;
namespace Network { namespace Events { class DataReceived; } }

class MedFuzzClient
    : public EventListener<Network::Events::DataReceived>,
      public FuzzingEventHooks
{
    public:
        MedFuzzClient() = delete;
        MedFuzzClient(const MedFuzzClient&) = delete;
        virtual ~MedFuzzClient();
        void start();
        void handle_event(Network::Events::DataReceived ev) override;
        static std::shared_ptr<MedFuzzClient> create(uint16_t port);

    private:
        explicit MedFuzzClient(uint16_t port);
        std::shared_ptr<ServerSocket> m_control_server;
        std::shared_ptr<PeerSocket> m_connection;
        std::shared_ptr<MedFuzzClient> m_self;
};

#endif //MED_FUZZ_CLIENT_H
