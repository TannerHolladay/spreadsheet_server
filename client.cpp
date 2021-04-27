#include <cstring>
#include <iostream>
#include "client.h"
#include "spreadsheet.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

int client::clientCount = 0;

client::client(tcp::socket socket) : socket(std::move(socket)) {
    ID = clientCount;
    clientCount++;
}

client::~client() {
    socket.close();
}

// The handshake to get the client's name and spreadsheet to connect the user to the server
void client::doHandshake() {
    auto self(shared_from_this());
    socket.async_read_some(
            boost::asio::buffer(data, max_length),
            [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                    // If no information is received then repeat
                    if (bytes_transferred == 0) {
                        doHandshake();
                        return;
                    }

                    // Adds a null character at the end to indicate the end of the data received.
                    data[bytes_transferred] = '\0';

                    // Keep receiving data until a new line character is reached
                    buffer += data;
                    if (data[bytes_transferred - 1] != '\n') {
                        doHandshake();
                        return;
                    }

                    // Checks to see if the buffer is empty or only whitespace
                    if (!buffer.empty() &&
                        !std::all_of(buffer.begin(), buffer.end(), [](char c) { return std::isspace(c); })) {

                        // If the client's name isn't set, then it applies the buffer to the username. Otherwise, it joins the user to a spreadsheet.
                        if (this->userName.empty()) {
                            std::cout << "User joined: " << buffer << std::endl;
                            this->userName = buffer;
                            // After the username is received, the server sends the spreadsheets
                            if (!spreadsheet::spreadsheets.empty()) {
                                std::string message;
                                std::map<std::string, spreadsheet*>::iterator it;
                                for (it = spreadsheet::spreadsheets.begin();
                                     it != spreadsheet::spreadsheets.end(); it++) {
                                    message += it->first + "\n";
                                }
                                sendMessage(message);
                            } else {
                                // Sends two new line characters to indicate there are no created spreadsheets
                                sendMessage("\n");
                            }
                        } else {
                            // Creates a new spreadsheet if it does not exist
                            if (spreadsheet::spreadsheets.count(buffer) == 0) {
                                spreadsheet::spreadsheets[buffer] = new spreadsheet();
                                std::cout << "Created new spreadsheet: " << buffer << std::endl;
                            }
                            // Join the spreadsheet
                            currentSpreadsheet = spreadsheet::spreadsheets[buffer];
                            currentSpreadsheet->join(shared_from_this());
                            return;
                        }
                    }

                    buffer = ""; // Resets the buffer to be able to add more information
                    doHandshake();
                } else {
                    // Closes the socket connection if any errors happened when connecting
                    closeSocket(error);
                }
            }
    );
}

// Sends a message to the client
void client::sendMessage(const std::string& message) {
    auto self(shared_from_this());
    boost::asio::async_write(
            socket,
            boost::asio::buffer(message + "\n", message.size() + 1),
            [this, self](boost::system::error_code error, std::size_t) {
                if (error) {
                    closeSocket(error);
                }
            }
    );
}

// Event loop to process information into the spreadsheet from the client
void client::doRead() {
    auto self(shared_from_this());
    socket.async_read_some(
            boost::asio::buffer(data, max_length),
            [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                    // If no information is received then repeat
                    if (bytes_transferred == 0) {
                        doRead();
                        return;
                    }

                    // Adds a null character at the end to indicate the end of the data received.
                    data[bytes_transferred] = '\0';

                    // Keep receiving data until a new line character is reached
                    buffer += data;

                    // TODO: Handle event loop information here
                    // Maybe combine this method with doHandshake some way? Like an if statement or something else?
                    if (!buffer.empty() &&
                        !std::all_of(buffer.begin(), buffer.end(), [](char c) { return std::isspace(c); })) {
                        handleRawRequest(buffer);
                    }

                    buffer = ""; // Resets the buffer to be able to add more information
                    doRead();
                } else {
                    // Closes the socket connection if any errors happened when connecting
                    closeSocket(error);
                }
            }
    );
}

// UNUSED, do we need this?
// Get the spreadsheet that the client is connected to
spreadsheet* client::getCurrentSpreadsheet() {
    return this->currentSpreadsheet;
}

// Closes the socket
void client::closeSocket(boost::system::error_code error) {
    if (currentSpreadsheet != nullptr) {
        currentSpreadsheet->disconnect(shared_from_this());
    }
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    socket.close();
}

void client::handleRawRequest(const std::string request) {
    // Get the request type
    json jsonRequest = json::parse(request, nullptr, false);
    if (jsonRequest.is_discarded()) return;

    std::string requestType = jsonRequest["requestType"];
    if (requestType == "editCell") {
        std::string cellName = jsonRequest["cellName"];
        std::string cellContents = jsonRequest["contents"];
        if (getSelected() == cellName) {
            currentSpreadsheet->edit(cellName, cellContents, true);
        }
    } else if (requestType == "revertCell") {
        std::string cellName = jsonRequest["cellName"];
        currentSpreadsheet->revert(cellName);
    } else if (requestType == "selectCell") {
        std::string cellName = jsonRequest["cellName"];
        currentSpreadsheet->select(cellName, shared_from_this());
    } else if (requestType == "undo") {
        currentSpreadsheet->undo();
    }
}

std::string client::getSelected() {
    return currentSelectedCell;
}

void client::setSelectedCell(std::string cellName) {
    this->currentSelectedCell = cellName;
}

std::string client::getClientName() {
    return userName;
}
