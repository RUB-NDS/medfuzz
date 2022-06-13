/**
* ReplyWithData.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/


#include <cstdint>
#include <memory>
#include "Network/Events/DataReceived.h"
#include "Network/IO.h"
#include "ReplyWithData.h"

void ReplyWithData::handle_event(Network::Events::DataReceived e)
{
    e.connection()->write(m_data);
}
