/**
* ServerConnectionTests.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef SERVER_CONNECTION_TESTS_H
#define SERVER_CONNECTION_TESTS_H

#include "literals.h"
#include "ServerSocket.h"
#include "Network/Events/ClientConnected.h"
#include "Network/Events/ConnectionFailed.h"
#include "utils/FakeEventHandler.h"
#include "utils/TestConnection.h"

namespace Tests { namespace ServerSocket {
static const uint16_t SERVER_PORT = 12345;
static const std::string SERVER_HOSTNAME = "localhost";

TEST(ServerSocketTests, CreateWithExplicitPort)
{
    const uint16_t PORT = SERVER_PORT;
    auto sc = ::ServerSocket::create(PORT);
    EXPECT_EQ(sc->get_port(), PORT);
    EXPECT_TRUE(sc->is_closed());
    sc->listen();
    EXPECT_FALSE(sc->is_closed());
    sc->close();
    EXPECT_TRUE(sc->is_closed());
}

TEST(ServerSocketTests, CreateWithArbitraryPort)
{
    auto sc = ::ServerSocket::create(0);
    EXPECT_TRUE(sc->is_closed());
    EXPECT_EQ(sc->get_port(), 0);
    sc->listen();
    EXPECT_TRUE(sc->get_port() > 0_u16);
    EXPECT_TRUE(sc->get_port() <= 65535_u16);
    EXPECT_FALSE(sc->is_closed());
    sc->close();
    EXPECT_TRUE(sc->is_closed());
}

TEST(ServerSocketTests, ClientConnectedEvent)
{

    class ClientConnectedEventHandler
        : public Tests::Utils::FakeEventHandler<
            Network::Events::ClientConnected,
            Tests::Utils::TestConnection>
    {
        public:
            using Tests::Utils::FakeEventHandler<
                Network::Events::ClientConnected,
                Tests::Utils::TestConnection>::FakeEventHandler;
            virtual void check(Network::Events::ClientConnected ev) override
            {
                EXPECT_FALSE(data().client()->is_closed());
                EXPECT_EQ(ev.server(), data().server());
                EXPECT_FALSE(data().client()->is_closed());
                EXPECT_FALSE(ev.client()->is_closed());
                Log::info("Passed connection check.", Log::Verbosity::TRACE);
                data().client()->close();
                data().server()->close();
            }
    };

    class ClientConnectionFailedEventHandler
        : public Tests::Utils::FakeEventHandler<
            Network::Events::ConnectionFailed,
            Tests::Utils::TestConnection>
    {
        public:
            using Tests::Utils::FakeEventHandler<
            Network::Events::ConnectionFailed,
            Tests::Utils::TestConnection>::FakeEventHandler;

        virtual void check(Network::Events::ConnectionFailed ev) override
        {
            data().server()->close();
            FAIL();
        }


    };

    ::Tests::Utils::TestConnection connections(12345);
    auto server = connections.server();
    auto client = connections.client();
    auto clientConnectedEventHandler = std::make_shared<ClientConnectedEventHandler>(connections);
    server->client_connected_event().subscribe(
        clientConnectedEventHandler);
    auto clientConnectionFailedEventHandler =
    std::make_shared<ClientConnectionFailedEventHandler>(connections);
    client->connection_failed_event().subscribe(
        clientConnectionFailedEventHandler);
    server->listen();
    EXPECT_TRUE(clientConnectedEventHandler->events().empty());
    EXPECT_FALSE(server->is_closed());
    client->open();
    client->wait_exit();
    server->wait_exit();
}

TEST(ServerSocketTests, ClientDisconnectedEvent)
{

//     auto server = ::ServerSocket::create(SERVER_PORT);
//     auto client = PeerConnection::create(
//         SERVER_HOSTNAME, SERVER_PORT);
//     auto clientDisconnectedEventHandler =
//         std::make_shared<::Tests::Utils::FakeEventHandler<Network::Events::ClientDisconnected>>();
//     server->client_disconnected_event().subscribe(
//         clientDisconnectedEventHandler);
//     server->listen();
//     EXPECT_FALSE(server->is_closed());
//     EXPECT_FALSE(clientDisconnectedEventHandler->get_event().has_value());
//     client->listen();
//     EXPECT_FALSE(client->is_closed());
//     EXPECT_FALSE(clientDisconnectedEventHandler->get_event().has_value());
//     client->close();
//     EXPECT_TRUE(client->is_closed());
//     EXPECT_EQ(clientDisconnectedEventHandler->get_event()->server(), server);
//     EXPECT_EQ(clientDisconnectedEventHandler->get_event()->client(), client);
}

}}

#endif //SERVER_CONNECTION_TESTS_H
