#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../globals.h"
#include "customer_server.c"
#include "employees.c"
#include "admin.c"

typedef struct
{
    int server_fd;
    struct sockaddr_in address;
} ServerSocket;

ServerSocket setup_server()
{
    ServerSocket server;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.server_fd = -1;
    server.address.sin_addr.s_addr = INADDR_ANY;
    server.address.sin_family = AF_INET;
    server.address.sin_port = htons(SERVER_PORT);

    int opt = 1;

    if (server_fd == -1)
        return server;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(server_fd);
        printf("Could not set the socket options\n");
        return server;
    }

    if (bind(server_fd, (struct sockaddr *)&server.address, sizeof(server.address)) < 0)
    {
        close(server_fd);
        printf("Could not bind the socket to the port\n");
        return server;
    }

    if (listen(server_fd, 5) < 0)
    {
        close(server_fd);
        printf("Could not listen to any of the upcoming requests");
        return server;
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    server.server_fd = server_fd;

    return server;
}

void extract_args(int c, char **a, char *str)
{
    char delim[] = " ";
    char *saveptr;

    a[0] = __strtok_r(str, delim, &saveptr);

    for (int i = 1; i < c; i++)
    {
        a[i] = __strtok_r(NULL, delim, &saveptr);
    }
}

void handle_customer_requests(int sock_fd, Request *req)
{
    char *argv[req->argc];
    Response res;

    extract_args(req->argc, argv, req->arguments);

    printf("User Type: %d\n", req->user.user_type);
    printf("Argument 0: %s\n", argv[0]);

    res.user = req->user;

    switch (req->user.user_type)
    {
    case NONE:
        if (strcmp(argv[0], "LOGIN") != 0)
            break;
        if (strcmp(argv[1], "CUSTOMER") == 0)
            login_customer(argv[2], argv[3], &res);
        else if (strcmp(argv[1], "EMPLOYEE") == 0)
            login_employee(argv[2], argv[3], &res);
        else if (strcmp(argv[1], "ADMIN") == 0)
            login_admin(argv[2], argv[3], &res);
        else
            snprintf(res.body, RES_BODY_SIZE - 1, "\nINVALID REQUEST\n");
        break;
    
    case CUSTOMER:
        if (strcmp(argv[0], "GET_BALANCE") == 0) {
            printf("Getting Customer Balance\n");
            view_customer_balance(&req->user, &res);
        }
        break;
    default:
        snprintf(res.body, RES_BODY_SIZE - 1, "\nINVALID REQUEST\n");
    }

    if (send(sock_fd, &res, sizeof(Response), 0) < 0)
    {
        perror("Could not send the response to the request");
    }
}

int main()
{
    ServerSocket server = setup_server();
    int new_socket, addrlen = sizeof(server.address);

    if (server.server_fd == -1)
    {
        perror("Could not start the server");
        exit(EXIT_FAILURE);
    }

    // Accept a client connection
    if ((new_socket = accept(server.server_fd, (struct sockaddr *)&server.address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        close(server.server_fd);
        exit(EXIT_FAILURE);
    }

    Request req;
    Response res;

    // Receive and respond to multiple messages
    for (int i = 0; i < 10; i++)
    {
        // Receive a message
        if (read(new_socket, &req, sizeof(Request)) > 0)
            handle_customer_requests(new_socket, &req);
    }

    close(new_socket);
    close(server.server_fd);
    printf("Server terminated.\n");

    return 0;
}
