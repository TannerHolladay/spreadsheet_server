#ifndef CELL
#define CELL

#include <stack>

class cell{
	public:
		cell(char* contents);
		void updateContents(char* contents);
		char* getContents();
		void revertCell();
	private:
		std::stack<char*> revertStack;
		char* cellName;
		char* contents;
};
#endif