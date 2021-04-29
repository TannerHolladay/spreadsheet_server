#include "spreadsheet.h"
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>
#include <mutex>

using nlohmann::json;

// Initializes the static map of spreadsheets
std::map<std::string, spreadsheet*> spreadsheet::spreadsheets = std::map<std::string, spreadsheet*>();

spreadsheet::spreadsheet(const std::string& name) {
    // Initializes the set of clients
    clients = std::set<client::pointer>();
    // Initializes the map of cells
    cells = std::map<std::string, cell*>();
    // Load all the spreadsheets from saves if there any
    loadSpreadsheet(name);
    // Creates a saves directory if it doesn't exist
    boost::filesystem::create_directory("./saves/");
    // Create and open save with name of the spreadsheet
    _file.open("./saves/" + name + ".txt", std::ios_base::app);
}

spreadsheet::~spreadsheet() {
    // Closes the opened file
    _file.close();
    // Delete all cells in spreadsheet to release memory
    for (const auto& cell: cells) {
        delete cell.second;
    }
}

// Undoes the last action unless it's an undo
void spreadsheet::undo(std::string cellName) {
    // If undoStack is empty, then ignore action
    if (undoStack.empty()) return;
    // Lock when editing values on spreadsheet
    cellState undo = undoStack.top();
    // Swap cellName to be passed to the error catcher if a formula or circular happen
    cellName.swap(undo.first);
    // Try to edit the cell
    edit(cellName, undo.second, false);
    _mtx.lock();
    undoStack.pop();
    _mtx.unlock();
}

void spreadsheet::revert(const std::string& cellName) {
    // If cell hasn't been changed return
    if (cells.count(cellName) == 0 || !cells[cellName]->canRevert()) return;

    _mtx.lock();
    std::string oldContents = cells[cellName]->revert();

    undoStack.push(cellState(cellName, oldContents));
    _mtx.unlock();
}

void spreadsheet::edit(const std::string& cellName, const std::string& contents, bool canUndo) {
    _mtx.lock();
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
    _mtx.unlock();
}

void spreadsheet::select(const std::string& cellName, const client::pointer& client) {
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

void spreadsheet::join(const client::pointer& newClient) {
    _mtx.lock();
    // Send newClient the cells that other clients have selected
    for (const auto& client: clients) {
        json message = {
                {"messageType",  "cellSelected"},
                {"cellName",     client->getSelected()},
                {"selector",     std::to_string(client->getID())},
                {"selectorName", client->getClientName()}
        };
        newClient->sendMessage(message.dump());
    }
    clients.insert(newClient);
    _mtx.unlock();
    std::cout << "Joined spreadsheet" << std::endl;
    // Send spreadsheet information to client
    for (auto cell: cells) {
        json message = {
                {"messageType", "cellUpdated"},
                {"cellName",    cell.first},
                {"contents",    cell.second->getContents()}
        };
        newClient->sendMessage(message.dump());
    }
    newClient->sendMessage(std::to_string(newClient->getID()));
    newClient->doRead(); // Starts the loop that processes information from the client
}

// Send clients a message to tell them the server has shutdown and delete all sheets to release memory
void spreadsheet::serverShutdown(const std::string& message) {
    json jsonMessage = {
            {"messageType", "serverError"},
            {"message", message}
    };
    for (auto sheet: spreadsheet::spreadsheets) {
        sheet.second->sendMessage(jsonMessage.dump());
        delete sheet.second;
    }
}

//Saves every cell to a file
void spreadsheet::saveMessage(const std::string& message) {
    _file << message << std::endl;
}

// Load the saved spreadsheets
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

// Load a saved spreadsheet file
void spreadsheet::loadSpreadsheet(const std::string& name) {
    std::ifstream ssFile;
    ssFile.open("./saves/" + name + ".txt");
    std::cout << "Loading Spreadsheet: " << name << std::endl;

    std::string line;
    // Read each line of the file and process the message
    while (std::getline(ssFile, line)) {
        client::handleRawRequest(line, this, nullptr);
    }
    ssFile.close();
}

// When a client disconnects remove them from the spreadsheet and send others a message that they are gone
void spreadsheet::disconnect(const client::pointer& client) {
    if (clients.count(client) > 0) {
        _mtx.lock();
        clients.erase(client);
        _mtx.unlock();
        json jsonMessage = {
                {"messageType", "disconnected"},
                {"user", client->getID()}
        };
        sendMessageToOthers(jsonMessage.dump(), client->getID());
        std::cout << "Removed user: " + client->getClientName() + " from spreadsheet" << std::endl;
    }
    // Removes the client from this spreadsheet
}

// Send message to all clients in spreadsheet
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

cell* spreadsheet::getCell(const std::string& name) {
    return cells.count(name) != 0 ? cells[name] : nullptr;
}