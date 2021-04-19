#include <cstring>
#include <iostream>
#include "client.h"
#include "spreadsheet.h"

client::client(boost::asio::io_context& io_context) : socket(io_context) {

}

client::~client() {
    //socket.close??
    std::cout << "deleted" << std::endl;
}

std::string client::getCurrentSpreadsheet() {
    return this->currentSpreadsheet;
}

void client::sendMessage(std::string message) {
    boost::asio::async_write(
            socket,
            boost::asio::buffer(message),
            boost::bind(
                    &client::handleWrite,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
            )
    );
}

void client::setSelectedCell(std::string cellName) {
    this->currentSelectedCell = cellName;
}

void client::handleWrite(const boost::system::error_code&, size_t) {

}

void client::readMessage() {
    socket.async_read_some(
            boost::asio::buffer(data, max_length),
            boost::bind(
                    &client::handleMessage,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
            )
    );
}

void client::handleMessage(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        if (bytes_transferred == 0) {
            readMessage();
            return;
        }

        data[bytes_transferred] = '\0';

        buffer += data;

        if (data[bytes_transferred-1] != '\n') {
            readMessage();
            return;
        }

        std::cout << buffer << std::endl;

        if (userName == "") {
            userName = buffer;
            if (spreadsheet::spreadsheets.size() != 0){
                std::string message = "";
                for (auto key: spreadsheet::spreadsheets){
                    message += key.first+"\n";
                }
                sendMessage(message + "\n");
            } else {
                sendMessage("Create new Spreadsheet\n");
            }
        } else {
            if (spreadsheet::spreadsheets.count(buffer) == 0) {
                spreadsheet::spreadsheets[buffer] = new spreadsheet(buffer);
                std::cout << "Created new spreadsheet" << std::endl;
            }
            spreadsheet::spreadsheets[buffer]->join(shared_from_this());
            return;
        }

        buffer = "";

        readMessage();
    }
}

void client::connect() {
    readMessage();
}
