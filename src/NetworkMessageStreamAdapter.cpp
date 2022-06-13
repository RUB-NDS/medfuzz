/**
* NetworkMessageStreamAdapter.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "NetworkMessageStreamAdapter.h"
#include "EventProvider.h"
#include "Network/Events/DataReceived.h"
#include "Network/Events/DataSent.h"
#include "Network/IOEventProvider.h"
#include "RawStreamWriter.h"

void NetworkMessageStreamAdapter::handle_event(Network::Events::DataReceived e)
{
    m_stream_writer->write(e.data());
}

void NetworkMessageStreamAdapter::handle_event ( Network::Events::DataSent e)
{
    m_stream_writer->write(e.data());
}


void NetworkMessageStreamAdapter::attach(std::shared_ptr<Network::IOEventProvider> io_provider)
{
    if(m_mode != Mode::OUT)
    {
        io_provider->data_received_event().subscribe(shared_from_this());
    }

    if(m_mode != Mode::IN)
    {
        io_provider->data_sent_event().subscribe(shared_from_this());
    }
}


void NetworkMessageStreamAdapter::detach(std::shared_ptr<Network::IOEventProvider> io_provider)
{
    if(m_mode != Mode::OUT)
    {
        io_provider->data_received_event().unsubscribe(shared_from_this());
    }

    if(m_mode != Mode::IN)
    {
        io_provider->data_sent_event().unsubscribe(shared_from_this());
    }
}
