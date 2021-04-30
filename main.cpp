/**
 * Team: Arrays_Start_At_1
 * CS-3505 Spring 2021
 * University of Utah
 * Authors: Tanner Holladay, Abbey Nelson, Noah Carlson, Travis Schnider, Jimmy Glasscock, and Sergio Remigio
 *
 * Networking code based on async_tcp_echo_server.cpp and chat_server.cpp on https://www.boost.org/ from examples provided
 * by Christopher M. Kohlhoff. Using the boost library to handle networking.
 *
 * Used nlohmann json for JSON serialization. https://github.com/nlohmann/json
 *
 */
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include "client.h"
#include "spreadsheet.h"
#include <thread>
#include <boost/algorithm/string/case_conv.hpp>

using boost::asio::ip::tcp;

// The port to use for the server
#define PORT 1100

class tcp_server {
public:
    // Constructor for the tcp server that starts the method to accept connections
    explicit tcp_server(boost::asio::io_context& io_context) : acceptor(io_context, tcp::endpoint(tcp::v4(), PORT)) {
        std::cout << "Server Started" << std::endl;
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
        // Creates a new thread to watch for input
        std::thread t{
                [] {
                    spreadsheet::loadSpreadsheets();
                    std::string input;
                    while (std::cin >> input){
                        boost::algorithm::to_lower(input);
                        if (input == "stop" || input == "exit" || input == "quit" || input == "close") {
                            std::cout << "Shutting down server...\n";
                            spreadsheet::serverShutdown("Shutting down the server");
                            exit(0);
                        }
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
