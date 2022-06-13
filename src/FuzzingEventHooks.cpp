/**
* FuzzingEventHooks.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <string>
#include "EventProvider.h"
#include "File.h"
#include "FuzzingEventHooks.h"
#include "Settings.h"

void FuzzingEventHooks::setup_io_events(std::shared_ptr<Network::IOEventProvider> netio)
{
    if(Settings::get().is_echo_mode_enabled())
    {
        m_echo_handler = std::make_shared<EchoData>();
        netio->data_received_event().subscribe(m_echo_handler);
    }

    const std::string rdf_path = Settings::get().get_response_data_file();
    if(!rdf_path.empty())
    {
        m_reply_with_file_handler = std::make_shared<ReplyWithFileContents>(rdf_path);
        netio->data_received_event().subscribe(m_reply_with_file_handler);
    }

    if(Settings::get().terminates_on_receive())
    {
        m_terminate_on_receive_handler = std::make_shared<TerminateOnReceive>();
        netio->data_received_event().subscribe(m_terminate_on_receive_handler);
    }

    if(Settings::get().terminates_after_reply())
    {
        m_terminate_after_reply_handler = std::make_shared<TerminateAfterReply>();
        netio->data_sent_event().subscribe(m_terminate_after_reply_handler);
    }
}

void FuzzingEventHooks::client_init(std::shared_ptr<PeerSocket> ps)
{
    static const std::string& idf_path = Settings::get().get_initial_data_file();
    if(!idf_path.empty())
    {
        ps->write(File(idf_path).to_bytes());
    }
}
