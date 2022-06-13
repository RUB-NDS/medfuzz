/**
* FuzzingEventHooks.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef FUZZING_EVENT_HOOKS_H
#define FUZZING_EVENT_HOOKS_H

#include <memory>
#include "EchoData.h"
#include "Network/IOEventProvider.h"
#include "PeerSocket.h"
#include "ReplyWithFileContents.h"
#include "TerminateAfterReply.h"
#include "TerminateOnReceive.h"

class FuzzingEventHooks
{
    public:
        FuzzingEventHooks()
            : m_echo_handler(nullptr), m_reply_with_file_handler(nullptr),
              m_terminate_on_receive_handler(nullptr),
              m_terminate_after_reply_handler(nullptr) {}

    protected:
        void setup_io_events(std::shared_ptr<Network::IOEventProvider> netio);
        void client_init(std::shared_ptr<PeerSocket> ps);

    private:
        std::shared_ptr<EchoData> m_echo_handler;
        std::shared_ptr<ReplyWithFileContents> m_reply_with_file_handler;
        std::shared_ptr<TerminateOnReceive> m_terminate_on_receive_handler;
        std::shared_ptr<TerminateAfterReply> m_terminate_after_reply_handler;
};

#endif //FUZZING_EVENT_HOOKS_H


