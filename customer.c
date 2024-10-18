#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "globals.h"

void login_customer(char *username, char *password, Response *res)
{
    Customer cust;

    Bool logged_in = False;
    int fd = open(CUSTOMERS_FILE, O_RDONLY);

    while (read(fd, &cust, sizeof(Customer)) > 0)
    {
        if (strcmp(username, cust.username))
            continue;
        if (!strcmp(password, cust.password))
            logged_in = True;
        break;
    }

    if (!logged_in)
    {
        close(fd);
        snprintf(res->body, RES_BODY_SIZE - 1, "Invalid Username / Password\n");
        return;
    }

    res->user.user_type = CUSTOMER;
    res->user.user_id = cust.customer_id;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");

    close(fd);
}