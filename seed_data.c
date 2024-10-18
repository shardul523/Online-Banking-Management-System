#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"

int main()
{
    int customers_fd = open("data/CUSTOMERS_DB", O_CREAT | O_RDWR, 0766);
    int employees_fd = open("data/EMPLOYEES_DB", O_CREAT | O_WRONLY, 0666);
    int admin_fd = open("data/ADMIN_DB", O_CREAT | O_WRONLY, 0666);

    if (customers_fd == -1 || employees_fd == -1 || admin_fd == -1)
    {
        perror("Could not open the db files");
        exit(EXIT_FAILURE);
    }

    Customer customers[] = {
        {1, 100.0, "user1", "test1234"},
        {2, 200.5, "user2", "test1234"},
        {3, 300.25, "user3", "test1234"}};

    Employee employees[] = {
        {MANAGER, 1, "emp1", "test1234"},
        {REGULAR, 2, "emp2", "test1234"}};

    Admin admin = {1, "sanal", "test1234"};

    if (write(customers_fd, customers, sizeof(customers)) <= 0)
    {
        perror("Could not write to the database");
        exit(EXIT_FAILURE);
    }

    write(employees_fd, &employees, sizeof(employees));
    write(admin_fd, &admin, sizeof(admin));

    close(customers_fd);
    close(employees_fd);
    close(admin_fd);
}