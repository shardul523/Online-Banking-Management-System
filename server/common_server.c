#ifndef COMMON_SERVER_H
#define COMMON_SERVER_H

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "../globals.h"

void logout(Response *res) {
    res->user.user_id = -1;
    res->user.user_type = NONE;
    strcpy(res->user.username, "");
    strcpy(res->body, "Logout successful\n");
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

#endif