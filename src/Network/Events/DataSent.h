/**
* Network/Events/DataSent.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENTS_DATA_SENT_H
#define NETWORK_EVENTS_DATA_SENT_H

#include <cstdint>
#include <memory>
#include <vector>
#include "Network/Events/DataExchanged.h"

namespace Network {
    class IO;
    namespace Events {

class DataSent : public virtual DataExchanged
{
    public:
        DataSent() = delete;
        DataSent(std::shared_ptr<Network::IO> c, std::vector<uint8_t> data);
        virtual ~DataSent();
};

}}

#endif //NETWORK_EVENTS_DATA_SENT_H

