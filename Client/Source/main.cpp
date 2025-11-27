#include "Client.hpp"

using namespace IMChat::Client;

int32_t main(int32_t argc, char** argv)
{
    auto client = Client(IMCHAT_SERVER_IP, IMCHAT_SERVER_PORT);
    client.Run();
}
