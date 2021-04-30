#ifndef SHEET
#define SHEET

#include <map>
#include <stack>
#include "cell.h"
#include "client.h"
#include <regex>
#include <fstream>
#include <mutex>

typedef std::pair<std::string, std::string> cellState;

class spreadsheet {
public:
    explicit spreadsheet(const std::string& name);

    ~spreadsheet();

    static void serverShutdown(const std::string& message);

    void saveMessage(const std::string& message);

    static void loadSpreadsheets();

    void undo(std::string cellName);

    void revert(const std::string& cellName);

    void edit(const std::string& cellName, const std::string& contents, bool canUndo);

    void select(const std::string& cellName, const client::pointer& client);

    void sendMessage(const std::string& message);

    void sendMessageToOthers(const std::string& message, int id);

    void join(const client::pointer& client);

    void disconnect(const client::pointer& client);

    cell* getCell(const std::string& name);

    std::set<client::pointer> clients;

    static std::map<std::string, spreadsheet*> spreadsheets;

    void loadSpreadsheet(const std::string& name);

    std::mutex mtx;
private:

    std::ofstream _file;

    std::map<std::string, cell*> cells;

    std::stack<cellState> undoStack;
};

#endif
