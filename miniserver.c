#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include<stdbool.h>
#include<string.h>
#include "admin.h"
#include "professor.h"
#include "stud_tr.h"

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void *handleClient(void *arg);

int main() 
  {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddrLen = sizeof(clientAddress);

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) 
    {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) 
    {
        perror("Socket binding failed");
        exit(1);
    }

    // Listen for incoming client connections
    if (listen(serverSocket, MAX_CLIENTS) == -1)
    {
        perror("Listening failed");
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept and handle client connections
    while (1)
       {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Accepting connection failed");
            continue;
       }

        pthread_t clientThread;
        pthread_create(&clientThread, NULL, handleClient, (void *)&clientSocket);
    }

    // Close the server socket (never reached in this example)
    close(serverSocket);

    return 0;
}

void *handleClient(void *arg)
 {
    int clientSocket = *((int *)arg);
    char buffer[BUFFER_SIZE];

    char roleMenu[] = "Select your role:\n1. Admin\n2. Professor\n3. Student\n";
    send(clientSocket, roleMenu, strlen(roleMenu),0);
    // Receive and process data from the client
    while (1) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) 
          {
            close(clientSocket);
            pthread_exit(NULL);
          }
         buffer[bytesRead] = '\0';

        // Process the received data based on client role
      if (strstr(buffer, "admin") != NULL) 
         {

                if(admin_operation_handler(clientSocket))
                {
                  send(clientSocket,"Thank you", strlen("Thank you"),0);
                }
        } 
    else if (strstr(buffer, "professor") != NULL) 
     {
              prof_operation_handler(clientSocket);
    }

    else if (strstr(buffer, "student") != NULL) 
     {
                stud_operation_handler(clientSocket);
        }

        // Process the received data (e.g., echo it back)
       send(clientSocket, buffer, bytesRead, 0);
 
}
    pthread_exit(NULL);
}

