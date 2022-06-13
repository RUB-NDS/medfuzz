/**
* NetworkConnectionTest.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_CONNECTION_TEST_H
#define NETWORK_CONNECTION_TEST_H

#include <memory>
#include <chrono>
#include <thread>
#include "literals.h"
#include "Log.h"
#include "ServerSocket.h"
#include "PeerSocket.h"
#include "TerminalWriter.h"
#include "TerminateAfterReply.h"
#include "ReplyWithData.h"
#include "NetworkMessageStreamAdapter.h"

namespace NetworkConnectionTests {

TEST(NetworkConnectionTest, ConnectClientToServer)
{
    const std::string CLIENT_MSG = "Hello";
    const std::string SERVER_MSG = "World";
    
    auto serverSendStream = std::make_shared<StringWriter>();
    auto serverReceiveStream = std::make_shared<StringWriter>();
    auto serverReceiveAdapter = NetworkMessageStreamAdapter::create(
        serverReceiveStream, 
        NetworkMessageStreamAdapter::Mode::IN);
    auto serverSendAdapter = NetworkMessageStreamAdapter::create(
        serverSendStream,
        NetworkMessageStreamAdapter::Mode::OUT);
    auto serverReply = std::make_shared<ReplyWithData>(SERVER_MSG);
    auto serverTerminator = std::make_shared<TerminateAfterReply>("Server: ");
    auto sc { ServerSocket::create(0) };
    serverReceiveAdapter->attach(sc);
    serverSendAdapter->attach(sc);
    sc->data_received_event().subscribe(serverTerminator);
    sc->data_received_event().subscribe(serverReply);
    sc->data_sent_event().subscribe(serverTerminator);
    sc->listen();
    ASSERT_NE(sc->get_port(), 0_z);

    //std::this_thread::sleep_for(std::chrono::seconds(2));
    auto clientReceiveStream = std::make_shared<StringWriter>();
    auto clientSendStream = std::make_shared<StringWriter>();
    auto clientReceiveAdapter = NetworkMessageStreamAdapter::create(
        clientReceiveStream, 
        NetworkMessageStreamAdapter::Mode::IN);
    auto clientSendAdapter = NetworkMessageStreamAdapter::create(
        clientSendStream,
        NetworkMessageStreamAdapter::Mode::OUT);

    auto cc { PeerSocket::create("localhost", sc->get_port()) };
    clientReceiveAdapter->attach(cc);
    clientSendAdapter->attach(cc);
    auto clientTerminator = std::make_shared<TerminateAfterReply>("Client: ");
    cc->data_received_event().subscribe(clientTerminator);
    cc->data_sent_event().subscribe(clientTerminator);
    cc->open();
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    cc->write(CLIENT_MSG);
    cc->wait_exit();
    sc->close();
    sc->wait_exit();
    EXPECT_EQ(clientSendStream->to_string(), CLIENT_MSG);
    EXPECT_EQ(clientReceiveStream->to_string(), SERVER_MSG);
    EXPECT_EQ(serverSendStream->to_string(), SERVER_MSG);
    EXPECT_EQ(serverReceiveStream->to_string(), CLIENT_MSG);
}

}
#endif //NETWORK_CONNECTION_TEST_H
