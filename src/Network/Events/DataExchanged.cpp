/**
* Network/Events/DataExchanged.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <cstdint>
#include "Network/Events/DataExchanged.h"

namespace Network {
    namespace Events {

DataExchanged::DataExchanged(std::shared_ptr<Network::IO> c,
    std::vector<uint8_t> data)
    : m_connection(c), m_data(data)
{}

DataExchanged::~DataExchanged()
{}

const std::vector<uint8_t>& DataExchanged::data() const
{
    return m_data;
}

std::shared_ptr<Network::IO> DataExchanged::connection()
{
    return m_connection;
}

}}
