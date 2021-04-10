#ifndef CELL
#define CELL
class cell{
	public:
		void cell(char* contents);
		void updateContents(char* contents);
		char* getContents();
		void revertCell();
	private:
		std::stack<char*> revertStack;
		char* cellName;
		char* contents;
};
#endif