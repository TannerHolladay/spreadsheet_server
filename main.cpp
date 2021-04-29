/**
 * Code based on async_tcp_echo_server.cpp and chat_server.cpp on https://www.boost.org/
 */
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include "client.h"
#include "spreadsheet.h"
#include <thread>

using boost::asio::ip::tcp;

// The port to use for the server
#define PORT 1100

class tcp_server {
public:
    // Constructor for the tcp server that starts the method to accept connections
    tcp_server(boost::asio::io_context& io_context) : acceptor(io_context, tcp::endpoint(tcp::v4(), PORT)) {
        std::cout << "Server Started" << std::endl;
        spreadsheet::loadSpreadsheets();
        start_accept();
    }

private:
    // Loop to accepts clients to the server
    void start_accept() {
        acceptor.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<client>(std::move(socket))->doRead();
                    }

                    start_accept();
                }
        );
    }

    tcp::acceptor acceptor;
};

int main() {
    try {
        boost::asio::io_context io_context;
        tcp_server server(io_context);
        std::thread t{
            [] {
                std::string input;
                while (std::cin >> input)
                    if (input == "stop" || input == "Stop")
                    {
                        std::cout << "Shutting down server...\n";
                        //TODO SAVE SPREADSHEETS
                        spreadsheet::serverShutdown("Shutting down the server");
                        exit(0);
                    }
            }
        };
        t.detach();
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        spreadsheet::serverShutdown("Server is shutting down due to an error");
    }

    return 0;
}
