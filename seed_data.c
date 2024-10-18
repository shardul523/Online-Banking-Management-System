#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"

int main() {
    int customers_fd = open("data/CUSTOMERS_DB", O_CREAT | O_RDWR, 0766);
    // int employees_fd = open("DATABASE/EMPLOYEES_DB", O_CREAT | O_WRONLY, 0666);
    // int admin_fd = open("DATABASE/ADMIN_DB", O_CREAT | O_WRONLY, 0666);

    if (customers_fd == -1 ) {
        perror("Could not open the db files");
        exit(EXIT_FAILURE);
    }

    Customer customers[] = {
        {1, 100.0, "user1", "test1234"},
        {2, 200.5, "user2", "test1234"},
        {3, 300.25, "user3", "test1234"}
    };

    if (write(customers_fd, customers, sizeof(customers)) <= 0) {
        perror("Could not write to the database");
        exit(EXIT_FAILURE);
    }

    close(customers_fd);
}