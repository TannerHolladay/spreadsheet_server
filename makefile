all:
	g++ -std=c++11 main.cpp spreadsheet.cpp client.cpp cell.cpp graph.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread -lboost_filesystem

no-warnings:
	g++ -w *.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread

clean:
	rm -f *.out
	rm -f *.o