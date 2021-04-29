#ifndef CLIENT
#define CLIENT

#include <boost/asio.hpp>
#include <memory>
#include <regex>

using boost::asio::ip::tcp;

class spreadsheet;

class client : public std::enable_shared_from_this<client> {
public:
    typedef std::shared_ptr<client> pointer;

    client(tcp::socket socket);

    ~client();

    void doRead();

    void sendMessage(const std::string& data);

    void closeSocket();

    void setSelectedCell(const std::string& cellName);

    std::string getClientName();

    std::string getSelected();

    int getID() const;

    static void handleRawRequest(const std::string& request, spreadsheet* currentSpreadsheet, const client::pointer& client);

private:
    int ID;

    static int clientCount;

    std::string userName;

    //currentSpreadsheet - string containing the name of the spreadsheet the client is connected to
    spreadsheet* currentSpreadsheet;

    //this is the cell the client has selected
    std::string currentSelectedCell;

    //socket - be initialized by constructor
    tcp::socket socket;
    enum {
        max_length = 1024
    };
    char data[max_length];
    std::string buffer;
};

#endif
