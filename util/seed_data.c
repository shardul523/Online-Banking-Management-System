#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "../globals.h"

int main()
{
    int employees_fd = open(EMPLOYEES_FILE, O_CREAT | O_WRONLY, 0666);
    int customers_fd = open(CUSTOMERS_FILE, O_CREAT | O_RDWR, 0766);
    int records_fd = open(RECORDS_FILE, O_CREAT | O_WRONLY, 0666);
    int admin_fd = open(ADMIN_FILE, O_CREAT | O_WRONLY, 0666);

    if (customers_fd == -1 || employees_fd == -1 || admin_fd == -1)
    {
        perror("Could not open the db files");
        exit(EXIT_FAILURE);
    }

    Customer customers[] = {
        {1, 100.0, "user1", "test1234", True, False, ""},
        {2, 200.5, "user2", "test1234", True, False, ""},
        {3, 300.25, "user3", "test1234", True, False, ""},
        {4, 500, "user4", "test1234", False, False, ""}};

    Employee employees[] = {
        {MANAGER, 1, "emp1", "test1234"},
        {REGULAR, 2, "emp2", "test1234"}};

    Admin admin = {1, "sanal", "test1234"};

    Record record = {3, 2, 1, 0};

    if (write(customers_fd, customers, sizeof(customers)) <= 0)
    {
        perror("Could not write to the database");
        exit(EXIT_FAILURE);
    }

    write(employees_fd, &employees, sizeof(employees));
    write(admin_fd, &admin, sizeof(admin));
    write(records_fd, &record, sizeof(Record));

    close(customers_fd);
    close(employees_fd);
    close(admin_fd);
    close(records_fd);
}
