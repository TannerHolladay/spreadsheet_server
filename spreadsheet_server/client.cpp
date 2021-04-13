#include "client.h"
#include <cstring>

client::client(int soc, char* username){
	this->soc = soc;
	this->username = username;
}

client::~client(){
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