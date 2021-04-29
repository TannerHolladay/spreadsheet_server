#include <cstring>
#include <iostream>
#include "client.h"
#include "spreadsheet.h"
#include <regex>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <nlohmann/json.hpp>
#include <boost/algorithm/string/case_conv.hpp>

using nlohmann::json;

int client::clientCount = 0;

client::client(tcp::socket socket) : socket(std::move(socket)) {
    ID = clientCount;
    clientCount++;
}

client::~client() {
    closeSocket();
}

// The handshake to get the client's name and spreadsheet to connect the user to the server
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
                    if (data[bytes_transferred - 1] != '\n') {
                        doRead();
                        return;
                    }

                    buffer.pop_back();
                    std::cout << "Received: " << buffer << std::endl;

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
                                for (const auto& sheet: spreadsheet::spreadsheets) {
                                    message += sheet.first + "\n";
                                }
                                sendMessage(message);
                            } else {
                                // Sends two new line characters to indicate there are no created spreadsheets
                                sendMessage("\n");
                            }
                        } else if (currentSpreadsheet == nullptr) {
                            // Creates a new spreadsheet if it does not exist
                            if (spreadsheet::spreadsheets.count(buffer) == 0) {
                                spreadsheet::spreadsheets[buffer] = new spreadsheet(buffer);
                                std::cout << "Created new spreadsheet: " << buffer << std::endl;
                            }
                            // Join the spreadsheet
                            currentSpreadsheet = spreadsheet::spreadsheets[buffer];
                            currentSpreadsheet->join(shared_from_this());
                        } else {
                            handleRawRequest(buffer, currentSpreadsheet, shared_from_this());
                        }
                    }

                    buffer = ""; // Resets the buffer to be able to add more information
                    doRead();
                } else {
                    // Closes the socket connection if any errors happened when connecting
                    closeSocket();
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
                    closeSocket();
                }
            }
    );
}

// Closes the socket
void client::closeSocket() {
    if (!socket.is_open()) return;
    if (currentSpreadsheet != nullptr) {
        currentSpreadsheet->disconnect(shared_from_this());
        currentSpreadsheet->clientDisconnected(ID);
        currentSpreadsheet = nullptr;
    }
    socket.close();

    std::cout << "Socket closed" << std::endl;
}

void client::handleRawRequest(const std::string& request, spreadsheet* currentSpreadsheet, client::pointer client) {
    // Get the request type
    json jsonRequest = json::parse(request, nullptr, false);
    if (jsonRequest.is_discarded()) return;

    std::string requestType = jsonRequest["requestType"];
    std::string cellName = jsonRequest.count("cellName") != 0 ? jsonRequest.at("cellName"): "";
    boost::algorithm::to_upper(cellName);
    if (requestType == "selectCell") {
        if (client == nullptr) return;
        currentSpreadsheet->select(cellName, client);
    } else {
        try {
            if (client != nullptr) {
                currentSpreadsheet->saveMessage(request);
            }
            if (requestType == "editCell") {
                std::string cellContents = jsonRequest["contents"];
                if (client == nullptr || client->getSelected() == cellName) {
                    currentSpreadsheet->edit(cellName, cellContents, true);
                }
            } else if (requestType == "revertCell") {
                currentSpreadsheet->revert(cellName);
            } else if (requestType == "undo") {
                currentSpreadsheet->undo(cellName);
            }
        } catch (const char* message) {
            std::cout << "Error caught" << std::endl;
            json errorMessage = {
                    {"messageType", "requestError"},
                    {"cellName",    cellName},
                    {"message",     message}
            };
            client->sendMessage(errorMessage.dump());
        }
    }

}

std::string client::getSelected() {
    return currentSelectedCell;
}

void client::setSelectedCell(const std::string& cellName) {
    this->currentSelectedCell = cellName;
}

std::string client::getClientName() {
    return userName;
}
