#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../globals.h"

void login_admin(char *username, char *password, Response *res)
{
    Admin adm;
    Bool logged_in = False;
    int fd = open(ADMIN_FILE, O_RDONLY);

    if (fd == -1)
    {
        printf("Admin file could not be opened\n");
        return;
    }

    while (read(fd, &adm, sizeof(Admin)) > 0)
    {
        if (strcmp(username, adm.username))
            continue;
        if (!strcmp(password, adm.password))
            logged_in = True;
        break;
    }

    if (!logged_in)
    {
        strcpy(res->body, "Invalid username / password\n");
        close(fd);
        return;
    }

    printf("Logged in successfully\n");

    res->user.user_id = adm.admin_id;
    res->user.user_type = ADMIN;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");

    close(fd);
}