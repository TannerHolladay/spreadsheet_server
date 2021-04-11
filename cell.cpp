#include "cell.h"

cell::cell(char* contents){
	this->contents = contents;
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