all:
	g++ *.cpp -L /usr/lib/ -lboost_system -lboost_thread -lpthread

clean:
	rm -f *.out
	rm -f *.o