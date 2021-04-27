#include "spreadsheet.h"
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using nlohmann::json;

std::map<std::string, spreadsheet*> spreadsheet::spreadsheets = std::map<std::string, spreadsheet*>();

spreadsheet::spreadsheet() {
}

void spreadsheet::undo() {
    if (undoStack.empty()) return;
    cellState undo = undoStack.top();
    undoStack.pop();

    edit(undo.first, undo.second, false);
}

void spreadsheet::revert(std::string cellName) {
    // If cell hasn't been changed return
    if (cells.count(cellName) == 0 || !cells[cellName].canRevert()) return;

    std::string newContents = cells[cellName].revert();
    json message = {
            {"messageType", "cellUpdated"},
            {"cellName",    cellName},
            {"contents",    newContents}
    };
    sendMessage(message.dump());
}

void spreadsheet::edit(std::string cellName, std::string contents, bool canUndo) {
    //if cellName not in cells
    if (cells.count(cellName) == 0) {
        //create a cell
        cells[cellName] = cell("", cellName);
    }

    if (canUndo) {
        undoStack.push(cellState(cellName, cells[cellName].getContents()));
    }

    cells[cellName].updateContents(contents);
    json message = {
            {"messageType", "cellUpdated"},
            {"cellName",    cellName},
            {"contents",    contents}
    };
    sendMessage(message.dump());
}

void spreadsheet::select(std::string cellName, client::pointer currentClient) {
    currentClient->setSelectedCell(cellName);
    // Update this when we add a JSON parser
    json message = {
            {"messageType",  "cellSelected"},
            {"cellName",     cellName},
            {"selector",     std::to_string(currentClient->ID)},
            {"selectorName", currentClient->getClientName()}
    };
    // Send the message to all clients except the one who made the request
    // clients don't have IDs right now, need to fix this
    sendMessageToOthers(message.dump(), currentClient->ID);
}

void spreadsheet::join(client::pointer client) {
    clients.insert(client);
    std::cout << "Joined spreadsheet" << std::endl;
    // Send spreadsheet information to client
    for (auto cell: cells) {
        json message = {
                {"messageType", "cellUpdated"},
                {"cellName",    cell.first},
                {"contents",    cell.second.getContents()}
        };
        client->sendMessage(message.dump());
    }
    client->sendMessage(std::to_string(client->ID));
    client->doRead(); // Starts the loop that processes information from the client
}

void spreadsheet::serverShutdown(std::string message) {
    json jsonMessage = {
            {"messageType", "serverError"},
            {"serverError", message}
    };
    for (auto sheet: spreadsheet::spreadsheets) {
        sheet.second->sendMessage(jsonMessage.dump());
    }
}

bool cmp(std::pair<std::string, cell>& a, std::pair<std::string, cell>& b)
{
    return a.first.substr(1, a.first.length()) < b.first.substr(1, b.first.length()) || a.first < b.first;
}

//Saves every cell to a file
void spreadsheet::saveToFile(){
    std::ofstream ssFile;
    ssFile.open("ServerSpreadsheets.txt");
    std::cout << "saving..." << std::endl;

    //loop through spreadsheets
    for(const auto& sheet: spreadsheet::spreadsheets){
        //adds spreadsheet name to file
        //ssFile << sheet->second->spreadsheetName << "\n";
        //loop through cells in each spreadsheet
        std::vector<std::pair<std::string, cell> > sortedCells;
        for (auto& it : sheet.second->cells) {
            sortedCells.push_back(it);
        }
        sort(sortedCells.begin(), sortedCells.end(), cmp);

        for(const auto& cell: sortedCells){
           // ssFile << cell.second.getContents();
            std::cout << cell.first << std::endl;

        }
    }
}

void spreadsheet::disconnect(client::pointer client) {
    if (clients.count(client) > 0) {
        clients.erase(client);
        std::cout << "Removed user: " + client->getClientName() + " from spreadsheet" << std::endl;
    }
    // Removes the client from this spreadsheet
}

void spreadsheet::sendMessage(std::string message) {
    for (const auto& client : clients) {
        client->sendMessage(message);
    }
}

// Send the message to all clients except the one who made the request
void spreadsheet::sendMessageToOthers(std::string message, int id) {
    for (const auto& client : clients) {
        if (client->ID != id) {
            client->sendMessage(message);
        }
    }
}