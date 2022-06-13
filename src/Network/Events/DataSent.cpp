/**
* Network/Events/DataSent.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <cstdint>
#include "Network/Events/DataSent.h"

namespace Network {
    namespace Events {

DataSent::DataSent(std::shared_ptr<Network::IO> c,
    std::vector<uint8_t> data)
    : DataExchanged(c, data)
{}

DataSent::~DataSent()
{}

}}
