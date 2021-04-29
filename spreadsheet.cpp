#include "spreadsheet.h"
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>

using nlohmann::json;

std::map<std::string, spreadsheet*> spreadsheet::spreadsheets = std::map<std::string, spreadsheet*>();

spreadsheet::spreadsheet(std::string name) {
    clients = std::set<client::pointer>();
    cells = std::map<std::string, cell*>();
    loadSpreadsheet(name);
    boost::filesystem::create_directory("./saves/");
    _file.open("./saves/" + name + ".txt", std::ios_base::app);
}

void spreadsheet::undo(std::string cellName) {
    if (undoStack.empty()) return;
    cellState undo = undoStack.top();
    undoStack.pop();
    cellName.swap(undo.first);
    edit(cellName, undo.second, false);
}

void spreadsheet::revert(const std::string& cellName) {
    // If cell hasn't been changed return
    if (cells.count(cellName) == 0 || !cells[cellName]->canRevert()) return;

    std::string oldContents = cells[cellName]->revert();

    undoStack.push(cellState(cellName, oldContents));
}

void spreadsheet::edit(const std::string& cellName, const std::string& contents, bool canUndo) {
    std::string oldContents;
    //if cellName not in cells
    if (cells.count(cellName) == 0) {
        //create a cell
        cells[cellName] = new cell(cellName, contents, this);
    } else {
        oldContents = cells[cellName]->getContents();
        cells[cellName]->updateContents(contents);
    }

    if (canUndo) {
        undoStack.push(cellState(cellName, oldContents));
    }
}

void spreadsheet::select(const std::string& cellName, client::pointer client) {
    client->setSelectedCell(cellName);
    // Update this when we add a JSON parser
    json message = {
            {"messageType",  "cellSelected"},
            {"cellName",     cellName},
            {"selector",     std::to_string(client->getID())},
            {"selectorName", client->getClientName()}
    };
    // Send the message to all clients except the one who made the request
    sendMessageToOthers(message.dump(), client->getID());
}

void spreadsheet::join(const client::pointer& client) {
    clients.insert(client);
    std::cout << "Joined spreadsheet" << std::endl;
    // Send spreadsheet information to client
    for (auto cell: cells) {
        json message = {
                {"messageType", "cellUpdated"},
                {"cellName",    cell.first},
                {"contents",    cell.second->getContents()}
        };
        client->sendMessage(message.dump());
    }
    client->sendMessage(std::to_string(client->getID()));
    client->doRead(); // Starts the loop that processes information from the client
}

void spreadsheet::serverShutdown(const std::string& message) {
    json jsonMessage = {
            {"messageType", "serverError"},
            {"message", message}
    };
    for (auto sheet: spreadsheet::spreadsheets) {
        sheet.second->sendMessage(jsonMessage.dump());
    }
}

void spreadsheet::clientDisconnected(int id)
{
    json jsonMessage = {
            {"messageType", "disconnected"},
            {"user", id}
    };
    sendMessageToOthers(jsonMessage.dump(), id);
}

//Saves every cell to a file
void spreadsheet::saveMessage(const std::string& message) {
    _file << message << std::endl;
}

void spreadsheet::loadSpreadsheets() {
    if (boost::filesystem::exists("./saves/") && boost::filesystem::is_directory("./saves/")) {
        std::cout << "Loading Spreadsheets..." << std::endl;
        // Iterates through all items in saves
        for (auto const& entry : boost::filesystem::directory_iterator("./saves/")) {
            if (boost::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt") {
                // Create a new spreadsheet
                std::string name = entry.path().filename().string();
                name = name.substr(0, name.size()-4);
                auto* sheet = new spreadsheet(name);
                // Add spreadsheet to list of spreadsheets on the server
                spreadsheets[name] = sheet;
            }
        }
        std::cout << "Spreadsheets Loaded" << std::endl;
    }
}

void spreadsheet::loadSpreadsheet(std::string name) {
    std::ifstream ssFile;
    ssFile.open("./saves/" + name + ".txt");
    std::cout << "Loading Spreadsheet: " << name << std::endl;

    std::string line;
    while (std::getline(ssFile, line)) {
        client::handleRawRequest(line, this, nullptr);
    }
    ssFile.close();
}

void spreadsheet::disconnect(client::pointer client) {
    if (clients.count(client) > 0) {
        clients.erase(client);
        std::cout << "Removed user: " + client->getClientName() + " from spreadsheet" << std::endl;
    }
    // Removes the client from this spreadsheet
}

void spreadsheet::sendMessage(const std::string& message) {
    for (const auto& client : clients) {
        client->sendMessage(message);
    }
}

// Send the message to all clients except the one who made the request
void spreadsheet::sendMessageToOthers(const std::string& message, int id) {
    if (clients.empty()) return;
    for (const auto& client : clients) {
        if (client->getID() != id) {
            client->sendMessage(message);
        }
    }
}

cell* spreadsheet::getCell(std::string name) {
    return cells.count(name) != 0 ? cells[name] : nullptr;
}