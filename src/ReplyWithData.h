/**
* ReplyWithData.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef REPLY_WITH_DATA_H
#define REPLY_WITH_DATA_H

#include <cstdint>
#include <string>
#include <vector>
#include "EventListener.h"

namespace Network { namespace Events {
    class DataReceived;
}}

class ReplyWithData : public EventListener<Network::Events::DataReceived>
{
    public:
        explicit ReplyWithData(const std::string& data)
            : ReplyWithData(std::vector<uint8_t>(data.begin(), data.end())) {}
        explicit ReplyWithData(std::vector<uint8_t> data) : m_data { data }{}
        virtual ~ReplyWithData() {}
        virtual void handle_event(Network::Events::DataReceived e) override;

    private:
        std::vector<uint8_t> m_data;

};

#endif //REPLY_WITH_DATA_H

