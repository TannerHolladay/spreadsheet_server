#ifndef CLIENT
#define CLIENT

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <sys/socket.h>

using boost::asio::ip::tcp;

class client : public boost::enable_shared_from_this<client> {
public:
    enum { max_length = 5 };
    typedef boost::shared_ptr<client> pointer;

    static pointer create(boost::asio::io_context& io_context) {
        return pointer(new client(io_context));
    }

    ~client();

    std::string getCurrentSpreadsheet();

    void sendMessage(std::string data);

    void handleMessage(const boost::system::error_code& error, std::size_t bytes_transferred);

    void setSelectedCell(std::string cellName);

    void connect();

    //socket - be initialized by constructor
    tcp::socket socket;
    char data[max_length];
    std::string buffer;
private:
    client(boost::asio::io_context& io_context);

    //currentSpreadsheet - string containing the name of the spreadsheet the client is connected to
    std::string currentSpreadsheet;
    //this is the cell the client has selected
    std::string currentSelectedCell;

    std::string userName;

    void handleWrite(const boost::system::error_code&, size_t);

    void readMessage();
};

#endif