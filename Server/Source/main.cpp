#include "Server.hpp"

#include "fmt/format.h"

using namespace IMChat::Server;

int32_t main(int32_t argc, char** argv)
{
    try
    {
        auto server = std::make_shared<Server>(IMCHAT_SERVER_PORT);
        server->Start();
        server->Run();
        server->Shutdown();
    }
    catch (const std::exception& e) {
        fmt::println("[SERVER] {}.", e.what());
    }
}
