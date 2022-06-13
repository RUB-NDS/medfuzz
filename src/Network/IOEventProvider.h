/**
* Network/IOEventProvider.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_IO_EVENT_PROVIDER_H
#define NETWORK_IO_EVENT_PROVIDER_H

#include "EventProvider.h"

namespace Network {
    namespace Events {
        class DataReceived;
        class DataSent;
    }

class IOEventProvider
{
    public:
        IEventProvider<Network::Events::DataSent>& data_sent_event();
        IEventProvider<Network::Events::DataReceived>& data_received_event();

    protected:
        IOEventProvider();
        virtual ~IOEventProvider();

        EventProvider<Network::Events::DataReceived> m_data_received_evp;
        EventProvider<Network::Events::DataSent> m_data_sent_evp;
};
}

#endif //NETWORK_IO_EVENT_PROVIDER_H
