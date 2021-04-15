#include <iostream>
#include "client.h"

void updateSelection(client c, char* cellName){
	//update the selectedCell field in the client object
	c.setSelectedCell(cellName);
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;

    //start loop to listen for new connections on another thread
    	//when a client is trying to connect, call the handshake

    //loop(for receiving data) - one thread per spreadsheet
    	//receive data
    	//decide to accept of reject data - is it valid data?

    	//if accepted, deserialize
    		//send to helper function to handle the kind of request

    	//send updated data to all of the clients connected to this spreadsheet
}