#ifndef CELL
#define CELL

#include <stack>

class cell {
public:
    cell();

    cell(std::string contents);

    void updateContents(std::string contents);

    std::string getContents();

    void revertCell();

private:
    std::stack<std::string> revertStack;
    std::string cellName;
    std::string contents;
};

#endif