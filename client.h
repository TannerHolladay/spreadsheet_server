#ifndef CLIENT
#define CLIENT

#include <sys/socket.h>

class client{
	public:
		void client(socket soc, char* spreadsheet);
		void ~client();
		char* getCurrentSpreadsheet();
		void sendData(char* data);
		void setSelectedCell(char* cellName);
	private:
		//socket - be initialized by constructor
		socket soc;
		//currentSpreadsheet - string containing the name of the spreadsheet the client is connected to
		char* currentSpreadsheet;
		//this is the cell the client has selected
		char* currentSelectedCell;
};
#endif