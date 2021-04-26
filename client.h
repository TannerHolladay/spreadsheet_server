#ifndef CLIENT
#define CLIENT

#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

class spreadsheet;

class client : public std::enable_shared_from_this<client> {
public:
    typedef std::shared_ptr<client> pointer;

    client(tcp::socket socket);

    ~client();

    void doHandshake();

    void sendMessage(const std::string& data);

    void doRead();

    void closeSocket(boost::system::error_code error);

    spreadsheet* getCurrentSpreadsheet();

    void setSelectedCell(std::string cellName);
    std::string getSelected();

private:
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