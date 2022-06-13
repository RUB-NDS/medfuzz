/**
* Network/IOEvents.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_EVENTS_DATA_EXCHANGED_H
#define NETWORK_EVENTS_DATA_EXCHANGED_H

namespace Network { namespace Events {

class DataExchangedEvent
{
    public:
        DataExchangedEvent() = delete;
        DataExchangedEvent(std::shared_ptr<NetworkIO> c, std::vector<uint8_t> data) :
            m_connection(c), m_data(data) {};
        virtual ~DataExchangedEvent() {};
        const std::vector<uint8_t>& data() const { return m_data; }
        std::shared_ptr<NetworkIO> connection() { return m_connection; }

    private:
        std::shared_ptr<NetworkIO> m_connection;
        std::vector<uint8_t> m_data;
};

}}

#endif //NETWORK_EVENTS_DATA_EXCHANGED_H


