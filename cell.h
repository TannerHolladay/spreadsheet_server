#ifndef CELL
#define CELL

#include <stack>

class cell{
	public:
		cell(char* contents, char* cellName);
		void updateContents(char* contents);
		char* getContents();
		void revertCell();
		bool operator==(const cell& c) const;
	private:
		std::stack<char*> revertStack;
		char* cellName;
		char* contents;
};
#endif