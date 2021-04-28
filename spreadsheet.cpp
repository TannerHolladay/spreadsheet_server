#include "spreadsheet.h"
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using nlohmann::json;

std::map<std::string, spreadsheet*> spreadsheet::spreadsheets = std::map<std::string, spreadsheet*>();

spreadsheet::spreadsheet() {
    dependencies = graph();
    cells = std::map<std::string, cell>();
}

void spreadsheet::undo(client::pointer currentClient) {
    if (undoStack.empty()) return;
    cellState undo = undoStack.top();
    undoStack.pop();
    edit(undo.first, undo.second, false, currentClient);
}

void spreadsheet::revert(std::string cellName, client::pointer currentClient) {
    // If cell hasn't been changed return
    if (cells.count(cellName) == 0 || !cells[cellName].canRevert()) return;

    undoStack.push(cellState(cellName, cells[cellName].getContents()));

    std::string newContents = cells[cellName].revert();

    //if we have a cycle, rollback
    if(checkCircularDependencies(cellName)){
        // not sure if this is right
        undo(currentClient);
        return;
    }

    json message = {
            {"messageType", "cellUpdated"},
            {"cellName",    cellName},
            {"contents",    newContents}
    };
    sendMessage(message.dump());
}

void spreadsheet::edit(std::string cellName, std::string contents, bool canUndo, client::pointer currentClient) {

    // Only for checking if a formula is valid
    if (contents.size() > 1 && contents[0] == '=') {
        try {
            std::string formula = contents.substr(1);
            isValidFormula(formula);
        }
        catch (std::string &message) {
            json errorMessage = {
                    {"messageType", "requestError"},
                    {"cellName",    cellName},
                    {"message",     message}
            };
            currentClient->sendMessage(errorMessage.dump());
        }
        if (checkCircularDependencies(cellName)) {
            json errorMessage = {
                    {"messageType", "requestError"},
                    {"cellName",    cellName},
                    {"message",     "Circular Dependency Detected"}
            };
            currentClient->sendMessage(errorMessage.dump());
            return;
        }
    }
    //if cellName not in cells
    if (cells.count(cellName) == 0) {
        //create a cell
        cells[cellName] = cell("", cellName);
    }

    if (canUndo) {
        undoStack.push(cellState(cellName, cells[cellName].getContents()));
    }

    cells[cellName].updateContents(contents);
    if (contents[0] = '=') {
        dependencies.replaceDependees(cellName, cells[cellName].getContentVariables());
    }
    else {
        dependencies.replaceDependees(cellName, std::vector<std::string>());
    }
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


/* isValidFormula: Determines whether a mathematical formula is syntactically correct.
 * Example:
 *     (1+1) //true
 *     (1    //false
 *
 * Param:    string - The formula to be validated.
 *
 * Returns:  bool   - True if valid. False if invalid.
 */
bool spreadsheet::isValidFormula(std::string formula) {
    std::vector<std::string> tokens;

    std::regex rgxTokens("([0-9]+(\\.[0-9]+)?|[a-zA-Z]+[0-9]+|[\\(\\)\\+\\-\\*/])");
    std::regex rgxDouble("([0-9]+(\\.[0-9]+)?)");
    std::regex rgxVariable("([a-zA-Z]+[0-9]+)");
    std::regex rgxAddSubtract("([\\+\\-])");
    std::regex rgxMultiplyDivide("([\\*/])");
    std::regex rgxLeftParen("(\\()");
    std::regex rgxRightParen("(\\))");
    std::regex rgxWhitespace("(\\s+)");

    if(formula.empty())
    {
        throw std::string("The formula is empty.");
    }

    tokens = tokenize(formula, rgxTokens);


    std::stack<std::string> ops;
    std::stack<double> vals;

    bool isValid = false;

    for(int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++) {

        std::string token = tokens[tokenIndex];

        std::string topOperator = !ops.empty() ? ops.top() : "";

        // Logic for double tokens
        if(std::regex_match(token, rgxDouble)) {
            if(topOperator == "*" || topOperator == "/") {
                vals.pop();
                ops.pop();
            }

            vals.push(0);
        }
            // Logic for values/variables like A1, BBX23 etc.
        else if(std::regex_match(token, rgxVariable)) {
            if(topOperator == "*" || topOperator == "/") {
                vals.pop();
                ops.pop();
            }

            vals.push(0);
        }
            // Logic for addition and subtraction tokens
        else if(std::regex_match(token, rgxAddSubtract)) {
            if(topOperator == "+" || topOperator == "-") {
                vals.pop();
                vals.pop();
                ops.pop();
                vals.push(0);
            }
            ops.push(token);
        }
            // Logic for multiply and divide tokens
        else if(std::regex_match(token, rgxMultiplyDivide)) {
            ops.push(token);
        }
            // Logic for left parenthesis token
        else if(std::regex_match(token, rgxLeftParen)) {
            ops.push(token);
        }
            // Logic for right parenthesis token
        else if(std::regex_match(token, rgxRightParen)) {
            if(topOperator == "+" || topOperator == "-") {
                vals.pop();
                vals.pop();
                ops.pop();
                vals.push(0);
            }

            topOperator = !ops.empty() ? ops.top() : "";

            if(topOperator == "(") {
                ops.pop();
            }
            else {
                throw std::string("Missing Parenthesis.");
            }

            topOperator = !ops.empty() ? ops.top() : "";

            if(topOperator == "*" || topOperator == "/") {
                vals.pop();
                vals.pop();
                ops.pop();
                vals.push(0);
            }
        }
        else if(std::regex_match(token, rgxWhitespace)) {
            throw "This expression has an unknown token: " + token + ".";
        }

    }
    // If the operators are empty there should be a final value on the value stack.
    if(ops.empty()) {
        if(vals.size() == 1) {
            isValid = true;
        }
        else {
            throw  std::string("Invalid expression.");
        }
    }
        // If there are two values left and the operator stack is not empty
        // then there is one operator left and it's a valid expression.
    else if(vals.size() == 2) {
        isValid = true;
    }
    else {
        throw  std::string("An extra operator was given in the expression.");
    }

    return isValid;
}

/* Tokenize: Creates a vector of tokens from an expression/formula.
 *
 * Param1:    string - The expression to be tokenized.
 * Param2:    regex  - The regular expression to match tokens.
 *
 * Returns:   vector - A vector containing tokens.
 */
std::vector<std::string> spreadsheet::tokenize(std::string expression, std::regex rgx){
    std::smatch matches;
    std::vector<std::string> tokenizedStrings;
    std::regex rgxEmptyOrWhite("(^$|\\s+)");

    // Search the expression for a token. When found, remove and search again.
    while(std::regex_search(expression, matches, rgx)){

        if(!regex_match(matches.str(1), rgxEmptyOrWhite)) {
            // Push match to list
            tokenizedStrings.push_back(matches.str(1));
            // Eliminate the previous match and create a new string to search
            expression = matches.suffix().str();
        }
    }

    return tokenizedStrings;
}

///  Determines if a circular dependency exists
/// \param cellName CellName to determine if a circular dependency exists. Example: "A4"
/// \return True if circular dependency, False if no circular dependency
bool spreadsheet::checkCircularDependencies(std::string cellName)
{
    // Recursively search for a circular dependency
    std::set<std::string> *visited = new std::set<std::string>();
    return visit(cellName, cellName, visited); // false = 0, true = 1
}

///
/// \param originalCellName Cell who we are searching if a circular dependency exists.
/// \param currentCellName Current cell that will have its contents checked
/// \param visited Set of seen cells
/// \return
bool spreadsheet::visit(std::string originalCellName, std::string currentCellName, std::set<std::string> *visited)
{
    std::cout << "Inside visit " << std::endl;
    std::set<std::string> *visited_set = visited;
    // Check in the current cell as visited
    visited_set->insert(currentCellName);
    // Get current cell's direct dependents
    std::unordered_set<std::string> directDependents = getDirectDependents(currentCellName);
    // Iterate over every direct dependent
    for(auto & directDependent : directDependents)
    {
        // If a circular exception was found
        if (directDependent == originalCellName)
        {
            return true;
        }
            // If dependent has not been visited yet
        else if(visited_set->count(directDependent) == 0)
        {
            // continue recursively searching for every dependent
            return visit(originalCellName, directDependent, visited_set);
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
std::vector<std::string> spreadsheet::getTokens(std::string cellName)
{
    std::string cellContents = cells[cellName].getContents();
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
std::unordered_set<std::string> spreadsheet::getDirectDependents(std::string cellName)
{
    return dependencies.getDependents(cellName);
    /** std::cout << "Inside directDependents " << std::endl;
    std::vector<std::string> directDependents = std::vector<std::string>();
    // Turn cellName into tokens
    std::vector<std::string> tokens = getTokens(cellName);
    std::regex reg("^[a-zA-Z]+[0-9]+$");
    //  Check which tokens are cellNames and add to directDependents
    std::smatch matches;
    for (auto & token : tokens)
    {
        // If current token is a cellName
        if (std::regex_search(token, matches, reg))
        {
            directDependents.push_back(token);
        }
    }
    return directDependents;
     */
}
