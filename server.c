#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8080
#define BUFFER_SIZE 512

struct Message
{
    int id;
    char text[256];
};

int main()
{
    int server_fd, new_socket, opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // Accept a client connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct Message msg;

    // Receive and respond to multiple messages
    for (int i = 0; i < 3; i++)
    {
        // Receive a message
        if (read(new_socket, &msg, sizeof(msg)) > 0)
        {
            printf("Received: [ID=%d] %s\n", msg.id, msg.text);

            // Modify the message and send it back
            msg.id += 10; // Modify ID for demonstration
            snprintf(msg.text, sizeof(msg.text), "Server Response %d", msg.id);

            // Send the modified message back to the client
            send(new_socket, &msg, sizeof(msg), 0);
        }
    }

    close(new_socket);
    close(server_fd);
    printf("Server terminated.\n");

    return 0;
}
