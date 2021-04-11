#include "spreadsheet.h"

void spreadsheet::spreadsheet(char* spreadsheetName){
	this->spreadsheetName = spreadsheetName;
}

void spreadsheet::updateCell(char* cellName, char* contents){
	if(cells[cellName] == NULL){
		//create a cell
		cells[cellName] = cell(contents);

		char* oldContents = "";

		undoStack.push({cellName, oldContents});

		return;
	}

	//update contents of the cell after fetching it
	cell c = cells[cellName];
	char* oldContents = c.getContents();

	undoStack.push({cellName, oldContents});

	c.updateContents(contents);
}

void spreadsheet::undo(){
	cellState state = undoStack.top();
	undoStack.pop();

	char* cellName = state.first;
	char* oldContents = state.second;

	cell c = cells[cellName];
	c.updateContents(oldContents);
}

void spreadsheet::revert(char* cellName){
	cell c = cells[cellName];
	char* oldContents = c.getContents();

	c.revertCell();

	//allow the revert to be undone
	undoStack.push({cellName, oldContents});
}