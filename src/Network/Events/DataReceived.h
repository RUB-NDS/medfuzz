/**
* Network/Events/DataReceived.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENTS_DATA_RECEIVED_H
#define NETWORK_EVENTS_DATA_RECEIVED_H

#include <cstdint>
#include <memory>
#include <vector>
#include "Network/Events/DataExchanged.h"

namespace Network {
    class IO;
    namespace Events {

class DataReceived : public virtual DataExchanged
{
    public:
        DataReceived() = delete;
        DataReceived(std::shared_ptr<Network::IO> c, std::vector<uint8_t> data);
        virtual ~DataReceived();
};

}}

#endif //NETWORK_EVENTS_DATA_RECEIVED_H
