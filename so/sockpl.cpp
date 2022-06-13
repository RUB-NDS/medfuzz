/**
* sockpl.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/


#include <cstdint>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include "PeerSocket.h"
#include "defaults.h"

#undef _GNU_SOURCE
#define _GNU_SOURCE
#define ENV_RESPONDER_HOST "F2S_RESPONDER_HOST"
#define ENV_RESPONDER_PORT "F2S_RESPONDER_PORT"
static constexpr const char* DEFAULT_RESPONDER_HOST  { "localhost" };
static constexpr const char* DEFAULT_RESPONDER_PORT { "12345" };

typedef struct sockaddr_in inetaddr_t;
typedef struct sockaddr sockaddr_t;
typedef int(*listen_fn_t)(int, int);

static listen_fn_t o_listen = (listen_fn_t)dlsym(RTLD_NEXT, "listen");
static const char* const RESPONDER_HOST = get_or_default(
    DEFAULT_RESPONDER_HOST, getenv(ENV_RESPONDER_HOST));
static const uint16_t RESPONDER_PORT =
    std::strtoull(get_or_default(DEFAULT_RESPONDER_PORT, getenv(ENV_RESPONDER_PORT)), nullptr, 10);

uint16_t get_port(int s)
{
    inetaddr_t inet_addr;
    socklen_t s_len;
    s_len = sizeof(in_addr);
    if(getsockname(s, (sockaddr*)&inet_addr, static_cast<socklen_t*>(&s_len)) < 0)
    {
        //return -1;
        throw std::runtime_error { "Socket error, port " + std::to_string(s) + " unkonwn." };
    }
    return ntohs(inet_addr.sin_port);
}

int listen(int sockfd, int backlog)
{
    uint16_t port = get_port(sockfd);
    std::cout << "Got port: " << port << std::endl;
    if(port > 0)
    {
        auto client = PeerSocket::create(RESPONDER_HOST, RESPONDER_PORT);
        client->write(std::to_string(port));
        client->close();
    }
    return o_listen(sockfd, backlog);
}

//int bind(int sockfd, const sockaddr_t* addr, socklen_t addrlen)
//{
//    int r = o_bind(sockfd, addr, addrlen);
//    if(r < 0)
//        std::cout << "Bind error: " << strerror(errno) << std::endl;
//    uint16_t port = ntohs(((inetaddr_t*)addr)->sin_port);
////    std::thread t { [port]()
////    {
////        start_server_mode(INPUT_FILE, port);
////    }};
////    t.detach();
//    ////if(addr->sa_family== AF_INET)
//    ////{
//        std::cout << "Port: " << port << std::endl;
//    ////}
//    return r;
//}



//int socket(int domain, int type, int protocol)
//{
//    //ClientConnection client(port, "localhost");
//    //client.subscribe(std::make_shared<TerminateOnReceive>());
//    //std::ifstream in(in_file, std::ios::binary);
//    //std::vector<uint8_t> data(std::istreambuf_iterator<char>(in), {});
//    //in.close();
//    //while(true)
//    //{
//    //    try
//    //    {
//    //        client.establish();
//    //        break;
//    //    }
//    //    catch(std::exception& e)
//    //    {
//    //        std::this_thread::sleep_for(std::chrono::milliseconds(1));
//    //    }
//    //}
//
//    //client.write(data);
//    //client.close();
//    int s = o_socket(domain, type, protocol);
//    std::cout << "Port: " << get_port(s) << std::endl;
//    return s;
//}
//
//


