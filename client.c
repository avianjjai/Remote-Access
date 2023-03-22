#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void close_connection(int socketFd);
void error_handling(char message[]);
void success_message(char message[]);

void ask(char ask_msg[], char response[]);

int main(int argc, char *argv[]) {
    // variables
    int socketFd;
    struct sockaddr_in serverAddress;

    // Check For Command Line Input
    if (argc < 3) {
        error_handling("Insufficient Command Line Arguments");
    }

    if (argc > 3) {
        error_handling("Too Few Arguments");
    }

    // Step 1: socket creation
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        error_handling("Socket Creation: Failed");
    } else {
        success_message("Socket Creation: Successful");
    }

    // Step 2: connection request
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(atoi(argv[2]));
    if (connect(socketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0) {
        error_handling("Connection: Failed");
    } else {
        success_message("Connection: Successful");
    }

    // Step 3: read()/write()
    char write_buffer[BUF_SIZE];
    char read_buffer[BUF_SIZE];
    while (1) {
        bzero(read_buffer, sizeof(read_buffer));
        read(socketFd, read_buffer, sizeof(read_buffer));
        ask(read_buffer, write_buffer);
        write(socketFd, write_buffer, sizeof(write_buffer));

        if (strcmp(write_buffer, "exit") == 0) {
            break;
        }
    }

    // Step 4: connection close
    close_connection(socketFd);
}

void close_connection(int socketFd) {
    close(socketFd);
    success_message("Socket: Closed");
    success_message("Thank You For Using !!!");
}

void error_handling(char message[]) {
    printf("%s\n", message);
    exit(1);
}

void success_message(char message[]) {
    printf("%s\n", message);
}

void ask(char ask_msg[], char command[]) {
    printf("%s", ask_msg);
    fgets(command, BUF_SIZE, stdin);
    command[strlen(command)-1] = '\0'; 
}