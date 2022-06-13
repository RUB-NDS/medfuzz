/**
* TerminateOnReceive.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TERMINATE_ON_RECEIVE_H
#define TERMINATE_ON_RECEIVE_H

#include "NetworkSocket.h"

class TerminateOnReceive
    : public EventListener<Network::Events::DataReceived>,
      public EventListener<Network::Events::DataSent>
{
    public:
        TerminateOnReceive() {}
        virtual ~TerminateOnReceive() {}
        virtual void handle_event(Network::Events::DataReceived e) override
        {
            e.connection()->close();
        }

        virtual void handle_event(Network::Events::DataSent e) override {};
};

#endif //TERMINATE_ON_RECEIVE_H
