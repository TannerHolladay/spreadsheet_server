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


///  Determines if a circular dependency exists
/// \param cellName CellName to determine if a circular dependency exists. Example: "A4"
/// \return True if circular dependency, False if no circular dependency
bool checkCircularDependencies(std::string cellName)
{
    // Recursively search for a circular dependency
    std::set<std::string> visited = std::set<std::string>();
    visit(cellName, cellName, &visited);
    return true; // false = 0, true = 1
}


///
/// \param originalCellName Cell who we are searching if a circular dependency exists.
/// \param currentCellName Current cell that will have its contents checked
/// \param visited Set of seen cells
/// \return
bool visit(std::string originalCellName, std::string currentCellName, std::set<std::string> *visited)
{
    std::cout << "Inside visit " << std::endl;
    std::set<std::string> visited_set = *visited;
    // Check in the current cell as visited
    visited_set.insert(currentCellName);
    // Get current cell's direct dependents
    std::vector<std::string> directDependents = getDirectDependents(currentCellName);
    // Iterate over every direct dependent
    for(int i = 0; i < directDependents.size(); i++)
    {
        // If a circular exception was found
        std::string cell = directDependents[i];
        if (cell==originalCellName)
        {
            return true;
        }
            // If dependent has not been visited yet
        else if(visited_set.find(directDependents[i]) != visited->end())
        {
            // continue recursively searching for every dependent
            visit(originalCellName, directDependents[i], &visited_set);
        }
    }
    // If no circular exception was found then return false
    return false;
}


/// Gets tokens from cell contents
/// An example:
/// Contents: "A4 + A16 + 1"
/// Tokens: ["A4", "A16", "1"]
/// \param cellName CellName from where contents will be extracted
/// \return A vector containing all tokens.
std::vector<std::string> getTokens(std::string cellName)
{
    std::string cellContents = cells[currentCell].getContents()
    std::cout << "Inside get tokens" << std::endl;
    std::vector<std::string> tokens = std::vector<std::string>();
    std::regex regSplit("(\\()|(\\))|(-)|(\\+)|(\\*)|(/)");
    //Split cellContents into tokens
    std::vector<std::string> tokens_vector =
            {
                    std::sregex_token_iterator(cellContents.begin(), cellContents.end(), regSplit, -1),
                    std::sregex_token_iterator()
            };

    // Remove whitespaces from tokens
    for (int i = 0; i < tokens_vector.size(); i++)
    {
        std::string trimmed_token = tokens_vector[i];
        trimmed_token.erase(remove(trimmed_token.begin(), trimmed_token.end(), ' '), trimmed_token.end());
        tokens_vector[i] = trimmed_token;
    }
    return tokens_vector;
}


/// Gets direct dependents from cell contents
/// \param cellName
/// \return A vector containing all of the cell dependents
std::vector<std::string> getDirectDependents(std::string cellName)
{
    std::cout << "Inside directDependents " << std::endl;
    std::vector<std::string> directDependents = std::vector<std::string>();
    // Turn cellName into tokens
    std::vector<std::string> tokens = getTokens(cellName);
    std::regex reg("^[a-zA-Z]+[0-9]+$");
    //  Check which tokens are cellNames and add to directDependents
    std::smatch matches;
    for (int i = 0; i < tokens.size(); i++)
    {
        // If current token is a cellName
        if (std::regex_search(tokens[i], matches, reg))
        {
            directDependents.push_back(tokens[i]);
        }
    }
    return directDependents;
}
