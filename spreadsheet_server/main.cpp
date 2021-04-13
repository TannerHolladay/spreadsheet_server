#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include "client.h"
#include <thread>

#define PORT 8080


void start_server(sockaddr_in address, int server_fd);

void updateSelection(client c, char *cellName) {
    //update the selectedCell field in the client object
    c.setSelectedCell(cellName);
}

int main() {
    int server_fd;
    struct sockaddr_in address = {};
    int opt = 1;
    int address_len = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // The localhost address
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::thread thread_object(start_server);
    start_server(address, server_fd);
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

void start_server(sockaddr_in address, int server_fd) {
    int new_socket;
    char buffer[1024] = {0};
    char *hello = const_cast<char *>("Hello from server");
    int addr_size = sizeof(address);
    while ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addr_size)) >= 0) {
        //read(new_socket, buffer, 1024);
        client *new_client = new client(new_socket, buffer);
        printf("%s\n", buffer);
        new_client->sendData(hello);
        printf("Hello message sent\n");
    }
    printf("Connection Error\n");
}