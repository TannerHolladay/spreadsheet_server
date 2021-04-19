#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "client.h"

using boost::asio::ip::tcp;

#define PORT 1100

class tcp_server {
public:
    explicit tcp_server(boost::asio::io_context& io_context)
            : io_context(io_context),
              acceptor(io_context, tcp::endpoint(tcp::v4(), PORT)) {
        std::cout << "Server Started" << std::endl;
        start_accept();
    }

private:
    void start_accept() {
        client::pointer new_connection = client::create(io_context);

        acceptor.async_accept(
                new_connection->socket,
                bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error)
        );
    }

    // Method for when a new client is connecting
    void handle_accept(boost::shared_ptr<client> new_connection, const boost::system::error_code& error) {
        if (!error) {
            new_connection->connect();
        }

        start_accept(); // Loops to connect more clients
    }

    tcp::acceptor acceptor;
    boost::asio::io_context& io_context;
};

int main() {
    try {
        boost::asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
