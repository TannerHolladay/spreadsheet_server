#include <client.h>

void client::client(socket soc, char* spreadsheet){
	this->soc = soc;
	this->currentSpreadsheet = spreadsheet;
}

void client::~client(){
	//socket.close??
}

char* client::getCurrentSpreadsheet(){
	return this->currentSpreadsheet;
}

void client::sendData(char* data){
	send(soc, data, strlen(data), 0);
}

void client::setSelectedCell(char* cellName){
	this->currentSelectedCell = cellName;
}