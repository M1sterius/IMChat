#include <print>
#include <cstdint>
#include <vector>
#include <iostream>
#include <thread>

#define ASIO_STANDALONE
#include "asio.hpp"

#include "Common.hpp"

void SendMsg(asio::ip::tcp::socket& socket, const std::string& message)
{
    socket.async_write_some(asio::buffer(message));
}

int32_t main(int32_t argc, char** argv)
{
    asio::error_code ec;
    asio::io_context context;

    auto idleWork = asio::make_work_guard(context);
    auto thread = std::thread([&context] { context.run(); });

    asio::ip::tcp::socket socket(context);

    ec = socket.connect(asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1"), SERVER_PORT), ec);

    if (!ec)
        std::println("Connected successfully!");
    else
    {
        std::println("Failed to connect to the server. Error: {}", ec.message());
        return 1;
    }

    if (socket.is_open())
    {
        std::string line;

        while (true)
        {
            std::getline(std::cin, line);
            socket.write_some(asio::buffer(line));
        }
    }

    context.stop();
    thread.join();
}
