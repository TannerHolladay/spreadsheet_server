#ifndef CELL
#define CELL

#include <stack>

class cell {
public:
    cell();

    cell(std::string contents, std::string cellName);

    void updateContents(std::string contents);

    std::string getContents() const;

    std::string revert();
    bool canRevert();

    bool operator==(const cell& c) const;

private:
    std::stack<std::string> revertStack;
    std::string cellName;
    std::string contents;
};

#endif