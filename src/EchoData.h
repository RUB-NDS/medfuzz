/**
* EchoData.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef ECHO_DATA_H
#define ECHO_DATA_H

#include "EventListener.h"

namespace Network { namespace Events {
    class DataReceived;
}}

class EchoData : public EventListener<Network::Events::DataReceived>
{
    public:
        EchoData() {}
        virtual ~EchoData() {}
        virtual void handle_event(Network::Events::DataReceived e) override;
};

#endif //ECHO_DATA_H

