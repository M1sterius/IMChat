#include "Client.hpp"

#include "fmt/format.h"

int32_t main(int32_t argc, char** argv)
{
    using namespace IMChat::Client;

    try
    {
        auto client = Client(IMCHAT_SERVER_IP, IMCHAT_SERVER_PORT);
        client.Run();
    }
    catch (const std::exception& e) {
        fmt::println("Failed to create IMChat client instance! Error: {}.", e.what());
    }
}
