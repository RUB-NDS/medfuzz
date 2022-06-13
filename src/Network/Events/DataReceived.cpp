/**
* Network/Events/DataReceived.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <cstdint>
#include "Network/Events/DataReceived.h"

namespace Network {
    namespace Events {

DataReceived::DataReceived(std::shared_ptr<Network::IO> c,
    std::vector<uint8_t> data)
    : DataExchanged(c, data)
{}

DataReceived::~DataReceived()
{}

}}
