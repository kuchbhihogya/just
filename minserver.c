#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "admin.h"
#include "professor.h"
#include "stud_tr.h"

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void *handleClient(void *arg);

int main() {
    // ... (the rest of your code remains the same)
}

void *handleClient(void *arg) {
    int clientSocket = *((int *)arg);
    char buffer[BUFFER_SIZE];

    char roleMenu[] = "Select your role:\n1. Admin\n2. Professor\n3. Student\n";
    send(clientSocket, roleMenu, strlen(roleMenu), 0);
    // Receive and process data from the client
    while (1) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // Handle disconnection or error
            close(clientSocket);
            pthread_exit(NULL);
        }
        buffer[bytesRead] = '\0';

        // Process the received data based on the client role
        if (strstr(buffer, "admin") != NULL) {
            if (admin_operation_handler(clientSocket)) {
                send(clientSocket, "Thank you", strlen("Thank you"), 0);
            }
        } else if (strstr(buffer, "professor") != NULL) {
            prof_operation_handler(clientSocket);
        } else if (strstr(buffer, "student") != NULL) {
            stud_operation_handler(clientSocket);
        }

        // Process the received data (e.g., echo it back)
        send(clientSocket, buffer, bytesRead, 0);
    }
    pthread_exit(NULL);
}
