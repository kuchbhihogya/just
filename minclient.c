#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    // Server address and port
    char* server_address = "127.0.0.1";  // Replace with your server's IP address
    int server_port = 12345;             // Replace with your server's port number

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, server_address, &serverAddr.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        exit(1);
    }

    printf("Connected to the server\n");

    while (1) {
        // Receive data from the server
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            // Connection closed by the server or an error occurred
            break;
        }

        // Process and display received data
        printf("Received: %s", buffer);

        // Send a response (optional)
        char response[1024];
        printf("Enter a message to send to the server: ");
        fgets(response, sizeof(response), stdin);
        send(clientSocket, response, strlen(response), 0);
    }

    // Close the socket
    close(clientSocket);
    return 0;
}
