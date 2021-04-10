#ifndef SHEET
#define SHEET

#include <map>

typedef std::pair<char*, char*> cellState;

class spreadsheet{
	public:
		void spreadsheet(char* spreadsheetName);
		void updateCell(char* cellName, char* contents);
		void undo();
		void revert(char* cellName);
	private:
		std::map<char*, cell> cells;
		char* spreadsheetName;

		std::stack<cellState> undoStack;
};
#endif