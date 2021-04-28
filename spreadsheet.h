#ifndef SHEET
#define SHEET

#include <map>
#include <stack>
#include <set>
#include "cell.h"
#include "client.h"
#include <regex>

typedef std::pair<std::string, std::string> cellState;

class spreadsheet {
public:
    spreadsheet();

    static void serverShutdown(std::string message);

    static void saveSpreadsheets();

    static void loadSpreadsheets();

    void undo();

    void revert(std::string cellName);

    void edit(std::string cellName, std::string contents, bool canUndo);

    void select(std::string cellName, client::pointer currentClient);

    void sendMessage(std::string message);

    void sendMessageToOthers(std::string message, int id);

    void join(client::pointer client);

    void disconnect(client::pointer client);

    void clientDisconnected(int id);

    bool checkCircularDependencies(std::string cellName);

    bool visit(std::string originalCellName, std::string currentCellName, std::set<std::string> *visited);

    std::vector<std::string> getDirectDependents(std::string cellName);

    std::vector<std::string> getTokens(std::string cellName);

    std::set<client::pointer> clients;
    static std::map<std::string, spreadsheet*> spreadsheets;

private:
    std::map<std::string, cell> cells;
    std::string spreadsheetName;

    std::stack<cellState> undoStack;
};

#endif
