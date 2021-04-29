#ifndef CELL
#define CELL

#include <stack>
#include <vector>
#include <regex>

class spreadsheet;

class cell {
public:
    cell();

    cell(std::string cellName, std::string contents, spreadsheet* spreadsheet);

    void updateContents(std::string contents);

    std::string getContents() const;

    std::string revert();

    bool canRevert();

    bool operator==(const cell& c) const;

    std::set<std::string> toContentVariables(std::string content);

    std::set<std::string> getContentVariables();

    void searchCircular(cell* originalCell, std::set<std::string> cellSet);

    std::set<std::string> contentVariables;
private:

    void sendUpdate(std::string newContents);

    bool isValidFormula(std::string formula);

    std::vector<std::string> tokenize(std::string expression, std::regex rgx);

    spreadsheet* _spreadsheet;
    std::stack<std::string> revertStack;
    std::string cellName;
    std::string contents;
};

#endif