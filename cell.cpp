#include <string>
#include "cell.h"

cell::cell(){
}

cell::cell(std::string contents){
	this->contents = contents;
}

void cell::updateContents(std::string contents){
	//push the old contents so we may revert
	revertStack.push(this->contents);

	this->contents = contents;
}

std::string cell::getContents(){
	return this->contents;
}

void cell::revertCell(){
	std::string oldContents = revertStack.top();
	revertStack.pop();

	this->contents = contents;
}