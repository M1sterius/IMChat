#include <print>
#include <cstdint>
#include <vector>
#include <iostream>
#include <thread>

#define ASIO_STANDALONE
#include "asio.hpp"

#include "Common.hpp"
#include "Server.hpp"

using namespace IMChat::Server;

int32_t main(int32_t argc, char** argv)
{
    auto server = Server(IMCHAT_SERVER_PORT);

    std::println("Press any button to terminate the server.");
    std::cin.get();
}
