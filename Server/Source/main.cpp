#include <print>
#include <cstdint>
#include <vector>
#include <iostream>
#include <thread>

#define ASIO_STANDALONE
#include "asio.hpp"

#include "Common.hpp"

void ReadData(const std::shared_ptr<asio::ip::tcp::socket>& socket, std::vector<char>& buffer)
{
    // Schedule a single read operation!!!
    socket->async_read_some(asio::buffer(buffer), [&socket, &buffer](const asio::error_code ec, const size_t size)
    {
        if (!ec)
        {
            std::print("Received message: ");
            for (size_t i = 0; i < size; i++)
                std::cout << buffer[i];
            std::cout << '\n';

            // Schedule another read operation in case the message wasn't received in full
            ReadData(socket, buffer);
        }
        else
            std::println("Error reading data: {}", ec.message());

    });
}

int32_t main(int32_t argc, char** argv)
{
    asio::io_context context;
    std::vector<char> buffer(KB(20));
    std::shared_ptr<asio::ip::tcp::socket> client;

    auto work = asio::make_work_guard(context);
    auto worker = std::thread([&context]{ context.run(); });

    asio::ip::tcp::acceptor acceptor(context, asio::ip::tcp::endpoint(SERVER_IP, SERVER_PORT));

    acceptor.async_accept([&buffer, &client](const asio::error_code& ec, asio::ip::tcp::socket socket)
    {
        if (!ec)
        {
            std::println("Client connected!");

            client = std::make_shared<asio::ip::tcp::socket>(std::move(socket));
            ReadData(client, buffer);
        }
    });

    std::println("Press any button to terminate the server.");
    std::cin.get();

    context.stop();
    worker.join();
}
