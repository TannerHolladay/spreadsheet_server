#include "spreadsheet.h"
#include <iostream>

std::map<std::string, spreadsheet*> spreadsheet::spreadsheets = std::map<std::string, spreadsheet*>();

spreadsheet::spreadsheet(std::string spreadsheetName) {
    this->spreadsheetName = spreadsheetName;
}

void spreadsheet::updateCell(std::string cellName, std::string contents) {
    //if cellName not in cells
    if(cells.find(cellName) == cells.end()){
        //create a cell
        cells[cellName] = cell(contents, cellName);

        std::string oldContents = "";

        undoStack.push(cellState(cellName, oldContents));

        return;
    }

    //update contents of the cell after fetching it
    cell c = cells[cellName];
    std::string oldContents = c.getContents();

    undoStack.push(cellState(cellName, oldContents));

    c.updateContents(contents);
}

void spreadsheet::undo() {
    cellState state = undoStack.top();
    undoStack.pop();

    std::string cellName = state.first;
    std::string oldContents = state.second;

    cell c = cells[cellName];
    c.updateContents(oldContents);
}

void spreadsheet::revert(std::string cellName) {
    cell c = cells[cellName];
    std::string oldContents = c.getContents();

    c.revertCell();

    //allow the revert to be undone
    undoStack.push(cellState(cellName, oldContents));
}

void spreadsheet::edit(std::string cellName, std::string contents) {
    updateCell(cellName, contents);
    std::string message = "{ messageType: \"cellUpdated\", cellName: " + cellName + " ,contents: " + contents + "}";
    for (auto client : spreadsheet::clients){
        if (client->ID != clientID){
            client->sendMessage(message);
        }
    }
}

void spreadsheet::select(std::string cellName, int clientID, std::string clientName) {
    // Update this when we add a JSON parser
    std::string message = "{ messageType: \"cellSelected\", cellName: " + cellName + ", selector: " + std::to_string(clientID) + ", selectorName: " + clientName + "}";
    // Send the message to all clients except the one who made the request
    // clients don't have IDs right now, need to fix this
    for (auto client: spreadsheet::clients){
        if (client->ID != clientID){
            client->sendMessage(message);
        }
    }
}

void spreadsheet::join(client::pointer client) {
    clients.insert(client);
    std::cout << "Joined spreadsheet" << std::endl;
    // Send spreadsheet information to client
    client->doRead(); // Starts the loop that processes information from the client
}

void spreadsheet::disconnect(client::pointer client) {
    if (clients.count(client) > 0){
        clients.erase(client);
        std::cout << "Removed user from spreadsheet" << std::endl;
    }
    // Removes the client from this spreadsheet
}
