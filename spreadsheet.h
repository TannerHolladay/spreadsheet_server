#ifndef SHEET
#define SHEET

#include <map>
#include <stack>
#include <set>
#include "cell.h"
#include "client.h"

typedef std::pair<std::string, std::string> cellState;

class spreadsheet {
public:
    spreadsheet(std::string spreadsheetName);

    void updateCell(std::string cellName, std::string contents);

    void undo();

    void revert(std::string cellName);

    std::set<client::pointer> clients;
    static std::map<std::string, spreadsheet*> spreadsheets;

    void join(client::pointer newClient);

private:
    std::map<std::string, cell> cells;
    std::string spreadsheetName;

    std::stack<cellState> undoStack;
};

#endif