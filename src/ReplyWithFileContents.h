/**
* ReplyWithFileContents.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef REPLY_WITH_WITH_FILE_CONTENTS_H
#define REPLY_WITH_WITH_FILE_CONTENTS_H

#include <string>
#include "Network/IO.h"
#include "EventListener.h"

namespace Network { namespace Events { class DataReceived; } }

class ReplyWithFileContents : public EventListener<Network::Events::DataReceived>
{
    public:
        explicit ReplyWithFileContents(const std::string& file_name)
            : m_file_name { file_name }{}
        virtual ~ReplyWithFileContents() {}
        virtual void handle_event(Network::Events::DataReceived e) override;

    private:
        std::string m_file_name;

};

#endif //REPLY_WITH_WITH_FILE_CONTENTS_H

