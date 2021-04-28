#ifndef CELL
#define CELL

#include <stack>
#include <vector>

class cell {
public:
    cell();

    cell(std::string contents, std::string cellName);

    void updateContents(std::string contents);

    std::string getContents();

    std::string revert();
    bool canRevert();

    bool operator==(const cell& c) const;

    std::vector<std::string> getContentVariables();

    std::vector<std::string> contentVariables;
private:
    std::stack<std::string> revertStack;
    std::string cellName;
    std::string contents;
    std::vector<std::string> tokenize(std::string expression, std::regex rgx);

};

#endif