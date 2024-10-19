#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "../globals.h"

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

int logout(int socket_fd, Token* user) {
    Request req;
    Response res;

    req.argc = 1;
    req.user = *user;
    strcpy(req.arguments, "LOGOUT");

    if (send(socket_fd, &req, sizeof(Request), 0) < 0) return -1;

    if (read(socket_fd, &res, sizeof(Response)) < 0) return -1;

    *user = res.user;

    printf("%s\n", res.body);

    return 0;
}

#endif