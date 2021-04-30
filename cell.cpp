#include <string>
#include <regex>
#include "cell.h"
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include "spreadsheet.h"

using nlohmann::json;

cell::cell() {

}

cell::cell(std::string cellName, std::string contents, spreadsheet* spreadsheet) {
    this->revertStack = std::stack<std::string>();
    this->cellName = cellName;
    this->_spreadsheet = spreadsheet;
    this->contentVariables = std::set<std::string>();
    updateContents(contents);
}

// Update a cell's contents
void cell::updateContents(std::string contents) {
    // Checks for circular/formula errors and then sends update to client. This is called first to avoid data being changed if an error occurs.
    sendUpdate(contents);
    //push the old contents so we may revert
    revertStack.push(this->contents);
    this->contents = contents;
}

bool cell::operator==(const cell& c) const {
    return ((this->contents == c.contents) && (this->cellName == c.cellName));
}

std::string cell::getContents() const {
    return this->contents;
}

std::string cell::revert() {
    sendUpdate(revertStack.top());
    revertStack.pop();

    std::string oldContents = this->contents;
    this->contents = oldContents;
    return oldContents;
}

void cell::sendUpdate(std::string newContents) {
    auto variables = std::set<std::string>();
    // Only for checking if a formula is valid
    if (newContents.size() > 1 && newContents[0] == '=') {
        std::string formula = newContents.substr(1);
        variables = toContentVariables(formula);
        searchCircular(cellName, variables);
        isValidFormula(formula);
    }

    contentVariables = variables;
    json message = {
            {"messageType", "cellUpdated"},
            {"cellName",    cellName},
            {"contents",    newContents}
    };
    _spreadsheet->sendMessage(message.dump());
}

bool cell::canRevert() {
    return !revertStack.empty();
}

std::set<std::string> cell::toContentVariables(std::string content) {
    auto variables = std::set<std::string>();

    std::smatch matches;
    std::regex rgxEmptyOrWhite("(^$|\\s+)");
    std::regex rgxVariable("([a-zA-Z]+[0-9]+)");

    while (std::regex_search(content, matches, rgxVariable)) {
        std::string match = matches.str(1);
        boost::algorithm::to_upper(match);
        // Push match to list
        variables.insert(match);
        // Eliminate the previous match and create a new string to search
        content = matches.suffix().str();
    }
    return variables;
}

std::set<std::string> cell::getContentVariables() {
    return contentVariables;
}

void cell::searchCircular(std::string originalCell, std::set<std::string> cellSet) {
    for (const std::string& dependent: cellSet) {
        if (dependent == originalCell) {
            throw "Circular dependency";
        }
        cell* cellObject = _spreadsheet->getCell(dependent);
        if (cellObject != nullptr) {
            searchCircular(originalCell, cellObject->getContentVariables());
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
bool cell::isValidFormula(std::string formula) {
    std::vector<std::string> tokens;

    std::regex rgxTokens("([0-9]+(\\.[0-9]+)?|[a-zA-Z]+[0-9]+|[\\(\\)\\+\\-\\*/])");
    std::regex rgxDouble("([0-9]+(\\.[0-9]+)?)");
    std::regex rgxVariable("([a-zA-Z]+[0-9]+)");
    std::regex rgxAddSubtract("([\\+\\-])");
    std::regex rgxMultiplyDivide("([\\*/])");
    std::regex rgxLeftParen("(\\()");
    std::regex rgxRightParen("(\\))");
    std::regex rgxWhitespace("(\\s+)");

    if (formula.empty()) {
        throw "The formula is empty.";
    }

    tokens = tokenize(formula, rgxTokens);


    std::stack<std::string> ops;
    std::stack<double> vals;

    bool isValid;

    for (auto token : tokens) {

        std::string topOperator = !ops.empty() ? ops.top(): "";

        // Logic for double tokens
        if (std::regex_match(token, rgxDouble)) {
            if (topOperator == "*" || topOperator == "/") {
                vals.pop();
                ops.pop();
                if (topOperator == "/" && token == "0") {
                    throw "Can't devide by zero";
                }
            }

            vals.push(0);
        }
            // Logic for values/variables like A1, BBX23 etc.
        else if (std::regex_match(token, rgxVariable)) {
            if (topOperator == "*" || topOperator == "/") {
                vals.pop();
                ops.pop();
            }

            vals.push(0);
        }
            // Logic for addition and subtraction tokens
        else if (std::regex_match(token, rgxAddSubtract)) {
            if (topOperator == "+" || topOperator == "-") {
                vals.pop();
                vals.pop();
                ops.pop();
                vals.push(0);
            }
            ops.push(token);
        }
            // Logic for multiply and divide tokens
        else if (std::regex_match(token, rgxMultiplyDivide)) {
            ops.push(token);
        }
            // Logic for left parenthesis token
        else if (std::regex_match(token, rgxLeftParen)) {
            ops.push(token);
        }
            // Logic for right parenthesis token
        else if (std::regex_match(token, rgxRightParen)) {
            if (topOperator == "+" || topOperator == "-") {
                vals.pop();
                vals.pop();
                ops.pop();
                vals.push(0);
            }

            topOperator = !ops.empty() ? ops.top(): "";

            if (topOperator == "(") {
                ops.pop();
            } else {
                throw "Missing Parenthesis.";
            }

            topOperator = !ops.empty() ? ops.top(): "";

            if (topOperator == "*" || topOperator == "/") {
                vals.pop();
                vals.pop();
                ops.pop();
                vals.push(0);
            }
        } else if (std::regex_match(token, rgxWhitespace)) {
            throw "This expression has an unknown token: " + token + ".";
        }

    }
    // If the operators are empty there should be a final value on the value stack.
    if (ops.empty()) {
        if (vals.size() == 1) {
            isValid = true;
        } else {
            throw "Invalid expression.";
        }
    }
        // If there are two values left and the operator stack is not empty
        // then there is one operator left and it's a valid expression.
    else if (vals.size() == 2) {
        isValid = true;
    } else {
        throw "An extra operator was given in the expression.";
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
std::vector<std::string> cell::tokenize(std::string expression, std::regex rgx) {
    std::smatch matches;
    std::vector<std::string> tokenizedStrings;
    std::regex rgxEmptyOrWhite("(^$|\\s+)");

    // Search the expression for a token. When found, remove and search again.
    while (std::regex_search(expression, matches, rgx)) {

        if (!regex_match(matches.str(1), rgxEmptyOrWhite)) {
            // Push match to list
            tokenizedStrings.push_back(matches.str(1));
            // Eliminate the previous match and create a new string to search
            expression = matches.suffix().str();
        }
    }

    return tokenizedStrings;
}