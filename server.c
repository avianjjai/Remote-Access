#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define CHANGE_DIRECTORY 1
#define EXECUTE 2

void close_connection(int serverSocketFd, int clientSocketFd);
void error_handling(char message[]);
void success_message(char message[]);

int type_of_command(char command[]);
void exec(char command[], char *output);
void curr_loc(char location[]);
void start_of_command(char *soc);
void subString_1(char s[], int size, char output[], int start, int length);
void subString_2(char s[], int size, char output[], int start);
//void str_to_chr(string s, char p[BUF_SIZE]);

int main(int argc, char *argv[]) {
    // variables
    int serverSocketFd, clientSocketFd;
    struct sockaddr_in serverAddress, clientAddress;
    int clientAddressLength;

    // Check For Command Line Input
    if (argc < 2) {
        error_handling("Insufficient Command Line Arguments");
    }

    if (argc > 2) {
        error_handling("Too Few Arguments");
    }

    // Step 1: socket creation
    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFd == -1) {
        error_handling("Socket Creation: Failed");
    } else {
        success_message("Socket Creation: Successful");
    }

    // Step 2: bind the socket (the process of allocating a port number to a sicket)
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(atoi(argv[1]));
    //serverAddress.sin_port = htons(5597);
    if (bind(serverSocketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0) {
        error_handling("Socket Binding: Failed");
    } else {
        success_message("Socket Binding: Successful");
    }

    // Step 3: Listning for Connection Request
    if (listen(serverSocketFd, 1) != 0) {
        error_handling("Listning: Failed");
    } else {
        success_message("Listning...");
    }

    // Step 4: accepting connection request
    bzero(&clientAddress, sizeof(clientAddress));
    clientSocketFd = accept(serverSocketFd, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressLength);
    if (clientSocketFd == -1) {
        error_handling("Connection: Denied");
    } else {
        success_message("Connection: Accept");
    }

    // Step 5: read()/write()
    char write_buffer[BUF_SIZE];
    char read_buffer[BUF_SIZE];
    char temp[BUF_SIZE];
    char command[BUF_SIZE];

    start_of_command(write_buffer);
    while (1) {
        write(clientSocketFd, &write_buffer, sizeof(write_buffer));

        bzero(read_buffer, sizeof(read_buffer));
        read(clientSocketFd, read_buffer, sizeof(read_buffer));
        strcpy(command, read_buffer);
        
        if (strcmp(command, "exit")==0) {
            break;
        }

        switch (type_of_command(command)) {
            case CHANGE_DIRECTORY: {
                subString_2(command, strlen(command), temp, 3);
                chdir(temp);
                start_of_command(write_buffer);

                //str_to_chr(exec(command), write_buffer);
                //write(clientSocketFd, write_buffer, sizeof(write_buffer));
                break;
            }

            case EXECUTE: {
                exec(command, write_buffer);
                start_of_command(temp);
                strcat(write_buffer, temp);
                break;
            }

            default: {
                break;
            }
        }
    }

    // Step 6: connection close
    close_connection(serverSocketFd, clientSocketFd);
}

void close_connection(int serverSocketFd, int clientSocketFd) {
    close(clientSocketFd);
    success_message("Client Socket: Closed");
    close(serverSocketFd);
    success_message("Server Socket: Closed");
}

void error_handling(char message[]) {
    printf("%s\n", message);
    exit(1);
}

void success_message(char message[]) {
    printf("%s\n", message);
}

int type_of_command(char command[]) {
    if (strlen(command)>=2 && command[0]=='c' && command[1]=='d')
        return CHANGE_DIRECTORY;
    else
        return EXECUTE;
}

void exec(char command[BUF_SIZE], char *result) {
    bzero(result, sizeof(result));
    char buffer[128];

    // Open pipe to file
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        return;
    }

    while (!feof(pipe)) {
        // use buffer to read and add to result
        if (fgets(buffer, 128, pipe) != NULL)
            strcat(result, buffer);
    }
    pclose(pipe);
}

void curr_loc(char location[]) {
    exec("pwd", location);
    location[strlen(location)-1] = '\0';
}

void start_of_command(char *soc) {
    char temp[BUF_SIZE];
    bzero(soc, sizeof(soc));
    strcat(soc, "(Remote) ");
    
    curr_loc(temp);
    strcat(soc, temp);
    strcat(soc, " $ ");
}

void subString_1(char s[], int size, char output[], int start, int length) {
    int readPointer = start;
    int writePointer = 0;
    if (length == -1) {
        length = size-start;
    }
    while (readPointer<size && writePointer<length) {
        output[writePointer] = s[readPointer];
        readPointer += 1;
        writePointer += 1;
    }
    output[writePointer] = '\0';
}

void subString_2(char s[], int size, char output[], int start) {
    int readPointer = start;
    int writePointer = 0;
    while (readPointer<size) {
        output[writePointer] = s[readPointer];
        readPointer += 1;
        writePointer += 1;
    }
    output[writePointer] = '\0';
}