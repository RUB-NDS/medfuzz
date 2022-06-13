/**
* Network/Events/DataExchanged.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENTS_DATA_EXCHANGED_H
#define NETWORK_EVENTS_DATA_EXCHANGED_H

#include <cstdint>
#include <memory>
#include <vector>

namespace Network {
    class IO;
    namespace Events {

class DataExchanged
{
    public:
        DataExchanged() = delete;
        DataExchanged(std::shared_ptr<Network::IO> c, std::vector<uint8_t> data);
        virtual ~DataExchanged();
        const std::vector<uint8_t>& data() const;
        std::shared_ptr<Network::IO> connection();

    private:
        std::shared_ptr<Network::IO> m_connection;
        std::vector<uint8_t> m_data;
};

}}

#endif //NETWORK_EVENTS_DATA_EXCHANGED_H

