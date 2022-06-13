/**
* ReplyWithFileContents.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <cstdint>
#include <vector>
#include "File.h"
#include "Log.h"
#include "Network/Events/DataReceived.h"
#include "ReplyWithFileContents.h"


void ReplyWithFileContents::handle_event(Network::Events::DataReceived e)
{
    Log::info("Reply", Log::Verbosity::TRACE);
    std::vector<uint8_t> data { File(m_file_name).to_bytes() };
    e.connection()->write(data);
}
