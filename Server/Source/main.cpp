#include "Server.hpp"

using namespace IMChat::Server;

int32_t main(int32_t argc, char** argv)
{
    auto server = Server(IMCHAT_SERVER_PORT);
    server.Run();
}
