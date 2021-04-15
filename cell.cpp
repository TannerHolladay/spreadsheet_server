#include "cell.h"

cell::cell(char* contents, char* cellName){
	this->contents = contents;
	this->cellName = cellName;
}

void cell::updateContents(char* contents){
	//push the old contents so we may revert
	revertStack.push(this->contents);

	this->contents = contents;
}

char* cell::getContents(){
	return this->contents;
}

void cell::revertCell(){
	char* oldContents = revertStack.top();
	revertStack.pop();

	this->contents = contents;
}

bool cell::operator==(const cell& c) const{
	return ((this->contents == c.contents) && (this->cellName == c.cellName));
}