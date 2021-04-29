all:
	g++ -std=c++11 -o ./spreadsheet_server main.cpp spreadsheet.cpp client.cpp cell.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread -lboost_filesystem

Test:
	g++ -std=c++11 Test.cpp spreadsheet.cpp client.cpp cell.cpp graph.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread -lboost_filesystem

no-warnings:
	g++ -w *.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread

clean:
	rm -f *.out
	rm -f *.o
