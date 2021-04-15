#ifndef CLIENT
#define CLIENT

#include <sys/socket.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class client{
	public:
		client(tcp::socket soc, char* spreadsheet);
		~client();
		char* getCurrentSpreadsheet();
		void sendData(char* data);
		void setSelectedCell(char* cellName);
	private:
		//socket - be initialized by constructor
		tcp::socket soc;
		//currentSpreadsheet - string containing the name of the spreadsheet the client is connected to
		char* currentSpreadsheet;
		//this is the cell the client has selected
		char* currentSelectedCell;
};
#endif