#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "globals.h"

void login_customer(char* username, char* password, Response *res) {
    Customer cust;
    
    Bool logged_in = False;
    int fd = open(CUSTOMERS_FILE, O_RDONLY);

    printf("FD: %d\n", fd);

    while (read(fd, &cust, sizeof(Customer)) > 0) {
        if (strcmp(username, cust.username)) continue;
        if (!strcmp(password, cust.password)) logged_in = True;
        break;
    }

    if (!logged_in) return;

    res->user.user_id = cust.customer_id;
    res->user.user_type = CUSTOMER;
    
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");
}