/**
* ReceiveDynamicTargetPort.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef RECEIVE_DYNAMIC_TARGET_PORT
#define RECEIVE_DYNAMIC_TARGET_PORT

#include <functional>
#include <stdexcept>
#include <algorithm>
#include "StringTools.h"
#include "Network/Events/DataReceived.h"
#include "Network/Events/DataSent.h"

/**
 * When preloading libsocketpl with the target application libsocketpl
 * will report port numbers bound by the target application.
 * ReceiveDynamicTargetPort receives the target port string converts it
 * to a uint16_t and forwards the port number to a callback function.
 **/
class ReceiveDynamicTargetPort
    : public EventListener<Network::Events::DataReceived>,
      public EventListener<Network::Events::DataSent>
{
    public:
        using port_callback_t = std::function<void(const uint16_t)>;
        ReceiveDynamicTargetPort(port_callback_t port_callback) :
           m_pcb{ port_callback } {}
        virtual ~ReceiveDynamicTargetPort() {}
        virtual void handle_event(Network::Events::DataReceived e)
        {
            uint16_t port = StringTools::from_string<uint16_t>(
                std::string { e.data().begin(), e.data().end() } );
            m_pcb(port);
        }
        virtual void handle_event(Network::Events::DataSent e) {}

    private:
        port_callback_t m_pcb;

};

#endif //RECEIVE_DYNAMIC_TARGET_PORT;
