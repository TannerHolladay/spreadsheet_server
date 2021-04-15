#include "client.h"

client::client(tcp::socket soc, char* spreadsheet){
	this->soc = std::move(soc);
	this->currentSpreadsheet = spreadsheet;
}

client::~client(){
	//socket.close??
}

char* client::getCurrentSpreadsheet(){
	return this->currentSpreadsheet;
}

void client::sendData(char* data){
	soc.send(boost::asio::buffer(data, strlen(data)));
}

void client::setSelectedCell(char* cellName){
	this->currentSelectedCell = cellName;
}