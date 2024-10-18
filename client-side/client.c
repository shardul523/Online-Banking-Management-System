#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "../globals.h"
#include "customer_client.c"
#include "employee_client.c"
#include "admin_client.c"

int login(int socket_fd, Token *user, char *username, char *password, UserType type);

int connect_server()
{
    struct sockaddr_in server_address;
    int socket_fd;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0)
        return -1;

    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        return -1;

    return socket_fd;
}

int main()
{
    int socket_fd = connect_server();

    if (socket_fd == -1)
    {
        perror("Could not connect to the server");
        exit(EXIT_FAILURE);
    }

    char username[USERNAME_SIZE], password[PASSWORD_SIZE];
    Token *user = (Token *)malloc(sizeof(Token));
    int user_type;

    user->user_id = -1;

    printf("LOGIN\n");
    printf("1. CUSTOMER\n");
    printf("2. EMPLOYEE\n");
    printf("3. ADMIN\n");

    printf("Enter user type: ");
    scanf("%d", &user_type);

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    switch (user_type)
    {
    case CUSTOMER:
        if (login(socket_fd, user, username, password, CUSTOMER) < 0)
            break;
        if (user->user_id != -1)
            customer_handler(socket_fd, user);
        break;
    case EMPLOYEE:
        login(socket_fd, user, username, password, EMPLOYEE);
        display_employee_menu();
        break;
    case ADMIN:
        login(socket_fd, user, username, password, ADMIN);
        display_admin_menu();
        break;
    default:
        break;
    }

    close(socket_fd);
}


int login(int socket_fd, Token *user, char *username, char *password, UserType type)
{
    Request login_req;
    Response login_res;

    login_req.argc = 4;
    login_req.user = *user;

    if (type == CUSTOMER)
        snprintf(login_req.arguments, MAX_ARGUMENT_SIZE - 1, "LOGIN CUSTOMER %s %s", username, password);
    else if (type == EMPLOYEE)
        snprintf(login_req.arguments, MAX_ARGUMENT_SIZE - 1, "LOGIN EMPLOYEE %s %s", username, password);
    else if (type == ADMIN)
        snprintf(login_req.arguments, MAX_ARGUMENT_SIZE - 1, "LOGIN ADMIN %s %s", username, password);

    if (send(socket_fd, &login_req, sizeof(Request), 0) < 0)
        return -1;

    if (read(socket_fd, &login_res, sizeof(Response)) < 0)
        return -1;

    *user = login_res.user;

    if (user->user_type != NONE)
        printf("%s\n", login_res.body);

    return 0;
}