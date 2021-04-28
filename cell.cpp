#include <string>
#include <regex>
#include "cell.h"

cell::cell() {
}

cell::cell(std::string contents, std::string cellName) {
    this->contents = contents;
    this->cellName = cellName;
}

void cell::updateContents(std::string contents) {
    //push the old contents so we may revert
    revertStack.push(this->contents);
    this->contents = contents;
}

bool cell::operator==(const cell& c) const {
    return ((this->contents == c.contents) && (this->cellName == c.cellName));
}

std::string cell::getContents() {
    return this->contents;
}

std::string cell::revert() {
    std::string oldContents = revertStack.top();
    revertStack.pop();

    this->contents = oldContents;
    return contents;
}

bool cell::canRevert() {
    return !revertStack.empty();
}

std::vector<std::string> cell::getContentVariables() {
    contentVariables = std::vector<std::string>();
    std::vector<std::string> tokens;
    std::regex rgxVariable("([a-zA-Z]+[0-9]+)");

    tokens = tokenize(contents, rgxVariable);

    for (int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++) {
        std::string token = tokens[tokenIndex];
        if (std::find(contentVariables.begin(), contentVariables.end(), rgxVariable) == contentVariables.end()) {
            contentVariables.push_back(token);
        }
    }
    return contentVariables;
}

std::vector<std::string> cell::tokenize(std::string expression, std::regex rgx){
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

