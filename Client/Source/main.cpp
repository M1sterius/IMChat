#include <print>
#include <cstdint>
#include <vector>
#include <iostream>
#include <thread>

#include "Common.hpp"
#include "Client.hpp"

using namespace IMChat::Client;

int32_t main(int32_t argc, char** argv)
{
    auto client = Client();

    if (!client.Connect(IMCHAT_SERVER_IP, IMCHAT_SERVER_PORT))
        return 1;

    std::string line;
    while (client.IsConnected())
    {
        std::getline(std::cin, line);
        client.SendTextMessage(line);
    }
}
