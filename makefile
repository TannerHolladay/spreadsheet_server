all:
	g++ *.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread

no-warnings:
	g++ -w *.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread

clean:
	rm -f *.out
	rm -f *.o