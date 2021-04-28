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
    spreadsheet();

    static void serverShutdown(std::string message);

    static void saveToFile();

    void undo(client::pointer currentClient);

    void revert(std::string cellName, client::pointer currentClient);

    void edit(std::string cellName, std::string contents, bool canUndo, client::pointer currentClient);

    void select(std::string cellName, client::pointer currentClient);

    void sendMessage(std::string message);

    void sendMessageToOthers(std::string message, int id);

    void join(client::pointer client);

    void disconnect(client::pointer client);

    void clientDisconnected(int id);

    std::set<client::pointer> clients;

    static std::map<std::string, spreadsheet*> spreadsheets;

private:
    std::map<std::string, cell> cells;

    std::string spreadsheetName;

    bool isValidFormula(std::string formula);

    std::vector<std::string> tokenize(std::string expression, std::regex rgx);

    std::stack<cellState> undoStack;
};

#endif