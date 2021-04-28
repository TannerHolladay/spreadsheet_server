#include "spreadsheet.h"
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>

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

bool cmp(std::pair<std::string, cell>& a, std::pair<std::string, cell>& b) {
    return a.first.substr(1, a.first.length()) < b.first.substr(1, b.first.length()) || a.first < b.first;
}

//Saves every cell to a file
void spreadsheet::saveSpreadsheets() {
    std::ofstream ssFile;
    std::cout << "saving..." << std::endl;
    boost::filesystem::create_directory("./saves/");

    //loop through spreadsheets
    for (const auto& sheet: spreadsheet::spreadsheets) {
        ssFile.open("./saves/" + sheet.first + ".csv");
        std::map<int, std::map<int, std::string>> vect;

        // Loops through each cell and adds them to a 2d map
        for (const auto& cell: sheet.second->cells) {
            // Converts the second part of the cellName to an integer
            int row = stoi(cell.first.substr(1, cell.first.length()));
            // Converts the first character to a column position
            int col = int(cell.first[0]) - 65;
            vect[row][col] = cell.second.getContents();
        }
        std::string output;
        for (int x = 0; x < vect.size(); ++x) {
            for (int y = 0; y < vect[x].size(); ++y) {
                // Adds contents of each cell and a comma to separate.
                ssFile << vect[x][y];
                ssFile << ",";
            }
            ssFile << std::endl;
        }
        ssFile.close();
    }
}

void spreadsheet::loadSpreadsheets() {
    std::ifstream ssFile;
    std::cout << "Loading Spreadsheets..." << std::endl;

    std::vector<boost::filesystem::path> paths;

    // Checks to see if saves folder exists
    if (boost::filesystem::exists("./saves/") && boost::filesystem::is_directory("./saves/")) {
        // Iterates through all items in saves
        for (auto const& entry : boost::filesystem::directory_iterator("./saves/")) {
            // Checks to make sure the file is a csv file and readable
            if (boost::filesystem::is_regular_file(entry) && entry.path().extension() == ".csv") {
                // Open file at path
                ssFile.open(entry.path().c_str());
                // Create a new spreadsheet
                auto* sheet = new spreadsheet();
                std::string name = entry.path().filename().string();
                // Add spreadsheet to list of spreadsheets on the server
                spreadsheets[name.substr(0, name.length() - 4)] = sheet;
                int row = 1;
                std::string contents, line;
                // Loops through every line in the file
                while (std::getline(ssFile, line)) {
                    // Starts the column at the first letter
                    char col = 'a';
                    // Goes through each character on the line
                    for (char character: line) {
                        if (character != ',') {
                            contents += character;
                        } else {
                            // Create a new cell if it's empty
                            if (!contents.empty()) {
                                sheet->edit(col + std::to_string(row), contents, false);
                                contents = "";
                            }
                            col++;
                        }
                    }
                    row++;
                }
            }
        }
    }
    std::cout << "Spreadsheets Loaded" << std::endl;
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