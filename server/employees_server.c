#include "common_server.c"
#include "../globals.h"

void login_employee(char *username, char *password, Response *res)
{
    Employee e;
    Bool logged_in = False;
    int fd = open(EMPLOYEES_FILE, O_RDONLY);

    if (fd == -1)
    {
        printf("Emp file could not be opened\n");
        return;
    }

    while (read(fd, &e, sizeof(Employee)) > 0)
    {
        if (strcmp(username, e.username))
            continue;
        if (!strcmp(password, e.password))
            logged_in = True;
        break;
    }

    if (!logged_in)
    {
        close(fd);
        strcpy(res->body, "Invalid username / password\n");
        return;
    }

    printf("Logged in successfully\n");

    res->user.user_id = e.employee_id;
    res->user.user_type = EMPLOYEE;
    res->user.role = e.role;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");

    close(fd);
}

void handle_employee_requests(char** argv, Response *res) 
{
    if (strcmp(argv[0], "LOGOUT") == 0)
    {
        logout(res);
    }
}