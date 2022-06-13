/**
* Network/IOEventProvider.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "Network/IOEventProvider.h"

namespace Network {
IOEventProvider::IOEventProvider()
    : m_data_received_evp(), m_data_sent_evp()
{}

IOEventProvider::~IOEventProvider()
{}

IEventProvider<Network::Events::DataSent>& IOEventProvider::data_sent_event()
{
    return m_data_sent_evp;
}

IEventProvider<Network::Events::DataReceived>& IOEventProvider::data_received_event()
{
    return m_data_received_evp;
}

}
