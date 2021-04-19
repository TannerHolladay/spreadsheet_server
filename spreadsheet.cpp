#include "spreadsheet.h"
#include <iostream>

std::map<std::string, spreadsheet*> spreadsheet::spreadsheets = std::map<std::string, spreadsheet*>();

spreadsheet::spreadsheet(std::string spreadsheetName) {
    this->spreadsheetName = spreadsheetName;
}

void spreadsheet::updateCell(std::string cellName, std::string contents) {
    if (cells.count(cellName) == 0) {
        //create a cell
        cells[cellName] = cell(contents);

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

void spreadsheet::join(client::pointer newClient) {
    clients.insert(newClient);
    std::cout << "Joined spreadsheet" << std::endl;
    // Send spreadsheet information to client
}
