/**
* medfuzz.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <memory>
#include "CLI11.hpp"
#include "Log.h"
#include "Settings.h"
#include "ServerSocket.h"
#include "MedFuzzServer.h"
#include "MedFuzzClient.h"

template<typename T,
         typename F = void(Settings::*)(const T&)>
static std::function<void(const T&)> make_fn(F f)
{
    return [=](const T& t) -> void { ((Settings::get()).*f)(t); };
}

static void define_args(CLI::App& args)
{
    constexpr uint16_t MIN_PORT = 0;
    constexpr uint16_t MAX_PORT = 65535;

    args.add_option_function(
        "-m,--mode",
        make_fn<Settings::Modes>(&Settings::set_mode),
        "Defines if MedFuzz acts a client or server."
    )->required()
     ->transform(CLI::CheckedTransformer(
         Settings::get_modes_map(), CLI::ignore_case));

    args.add_option_function(
        "-v,--verbosity",
        make_fn<Log::Verbosity>(&Settings::set_verbosity),
        "Sets the output verbosity level."
    )->transform(CLI::CheckedTransformer(
         Settings::get_verbosity_map(), CLI::ignore_case));

    args.add_option_function(
        "-c,--control-port",
        make_fn<uint16_t>(&Settings::set_control_port),
        "Port used to exchange control information. (Default: "
        + std::to_string(Settings::get().get_control_port()) + ")"
    )->check(CLI::Range(MIN_PORT, MAX_PORT));

    args.add_option_function(
        "-p,--target-port",
        make_fn<uint16_t>(&Settings::set_target_port),
        "Port used to connect to the fuzzing target."
    )->check(CLI::Range(MIN_PORT, MAX_PORT));

    args.add_flag_function(
        "-e,--echo",
        make_fn<int64_t>(&Settings::set_echo_mode_enabled),
        "Echos back any data received by the target."
    );

    args.add_option_function(
        "-i,--initial-data",
        make_fn<std::string>(&Settings::set_initial_data_file),
        "File containing data to be sent to the target immediately after a "
        "connection has been established with the peer."
    )->check(CLI::ExistingFile);

    args.add_option_function(
        "-r,--reply-data",
        make_fn<std::string>(&Settings::set_response_data_file),
        "File contating data to be sent to the target in reply to data "
        "received."
    )->check(CLI::ExistingFile);

    args.add_option_function(
        "-H,--host",
        make_fn<const std::string&>(&Settings::set_host_name),
        "Name of the host the target is running on (Default : "
        + Settings::get().get_host_name() + ")."
    );

    args.add_flag_function(
        "-d,--diconnect-after-send",
        make_fn<int64_t>(&Settings::set_terminate_after_reply),
        "Closes the network connection to the peer after the reply to "
        "the first incoming message from the peer has been sent."
    );

    args.add_flag_function(
        "-D,--disconnect-on-receive",
        make_fn<int64_t>(&Settings::set_terminate_on_receive),
        "Immediately closes the network connection to the peer once data "
        "has been received from peer."
    );
}

static void show_startup_info()
{
    std::stringstream ss;
    ss << "Remote control port:\t"  << Settings::get().get_control_port() << std::endl
       << "Target port:\t"   << Settings::get().get_target_port() << std::endl
       << "Initial data file:\t"  << Settings::get().get_initial_data_file() << std::endl
       << "Reply data file:\t"  << Settings::get().get_response_data_file() << std::endl
       << "Host:\t" << Settings::get().get_host_name() << std::endl
       << "Mode:\t" << (Settings::get().get_mode() == Settings::Modes::Client ? "Client" : "Server") << std::endl;
}

int main(int argc, char* argv[])
{
    CLI::App args {"MedFuzz"};
    define_args(args);
    CLI11_PARSE(args, argc, argv);
    Log::addTarget(Log::ConsoleOut(Settings::get().get_verbosity()));
    show_startup_info();

    if(Settings::get().get_mode() == Settings::Modes::Client)
    {
        auto client = MedFuzzClient::create(Settings::get().get_control_port());
        client->start();
    }
    else
    {
        auto server = MedFuzzServer::create(Settings::get().get_target_port());
        server->start();
    }
}
