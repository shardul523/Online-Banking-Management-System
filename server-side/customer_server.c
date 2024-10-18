#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../globals.h"

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


int get_customer(Token *user, Customer *cust) {
    int fd = open(CUSTOMERS_FILE, O_RDONLY);
    Bool found = False;

    if (fd == -1) return -1;

    while (read(fd, cust, sizeof(Customer)) > 0)
    {
        if (cust->customer_id != user->user_id) continue;
        found = True;
        break;
    }

    if (found) return 0;
    
    return -1;
}


void view_customer_balance(Token *user, Response* res) {
    Customer cust;

    if (get_customer(user, &cust) < 0) {
        snprintf(res->body, RES_BODY_SIZE - 1, "Could not get customer details\n");
        return;
    }

    snprintf(res->body, RES_BODY_SIZE - 1, "CUSTOMER BALANCE: %.2lf\n", cust.balance);
}