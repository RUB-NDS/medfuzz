/**
* TerminateAfterReply.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TERMINATE_AFTER_SEND_H
#define TERMINATE_AFTER_SEND_H

#include <atomic>
#include "EventListener.h"
#include "Network/IO.h"
#include "Network/Events/DataReceived.h"
#include "Network/Events/DataSent.h"
#include "Log.h"

class TerminateAfterReply
    : public EventListener<Network::Events::DataReceived>,
      public EventListener<Network::Events::DataSent>
{
    public:
       explicit TerminateAfterReply(const std::string& prefix)
           : m_sent{ false }, m_mtx{}, m_prefix{ prefix } {}
       TerminateAfterReply() : TerminateAfterReply("") {}
       virtual ~TerminateAfterReply() {}
       virtual void handle_event(Network::Events::DataReceived e) override
       {
           std::lock_guard<std::mutex> lock { m_mtx };
           if(m_sent)
           {
               Log::info("Terminate msg received: " + std::string(e.data().begin(),
                   e.data().end()), Log::Verbosity::TRACE);
               e.connection()->close();
               Log::info("Terminate", Log::Verbosity::TRACE);
           }
       }

       virtual void handle_event(Network::Events::DataSent e) override
       {
           std::lock_guard<std::mutex> lock { m_mtx };
           Log::info("Terminate msg sent: " + std::string(e.data().begin(),
               e.data().end()), Log::Verbosity::TRACE);
           m_sent = true;
       }
    private:
       bool m_sent;
       std::mutex m_mtx;
       const std::string m_prefix;
};

#endif //TERMINATE_AFTER_SEND_H
