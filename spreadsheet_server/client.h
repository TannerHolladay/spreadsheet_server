#ifndef CLIENT
#define CLIENT

#include <sys/socket.h>

class client{
	public:
		client(int soc, char* spreadsheet);
		~client();
		char* getCurrentSpreadsheet();
		void sendData(char* data);
		void setSelectedCell(char* cellName);
	private:
		//socket - be initialized by constructor
		int soc;
		//currentSpreadsheet - string containing the name of the spreadsheet the client is connected to
		char* currentSpreadsheet;
		char* username;
		//this is the cell the client has selected
		char* currentSelectedCell;
};
#endif