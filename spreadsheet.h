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

    static void serverShutdown();

    void updateCell(std::string cellName, std::string contents);

    void undo();

    void revert(std::string cellName);

    void edit(std::string cellName, std::string contents);

    void select(std::string cellName, client::pointer currentClient);

    std::set<client::pointer> clients;
    static std::map<std::string, spreadsheet*> spreadsheets;

    void join(client::pointer client);
    void disconnect(client::pointer client);

private:
    std::map<std::string, cell> cells;
    std::string spreadsheetName;

    std::stack<cellState> undoStack;
};

#endif