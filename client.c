#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define CUSTOMERS_FILE "customers_db"

enum Role
{
    USER,
    EMPLOYEE,
    MANAGER
};

typedef struct
{
    long long acc_no;
    char username[30];
    char password[15];
    enum Role role;
} Customer;

void init_db()
{
    int fd = open(CUSTOMERS_FILE, O_WRONLY);
    Customer customers[] = {
        {1, "shardul05", "test1234", MANAGER},
        {2, "sanal", "test1234", EMPLOYEE},
        {3, "alka158", "test1234", USER}};
    write(fd, customers, sizeof(customers));
    close(fd);
}

Customer *get_customer(char *user)
{
    int fd = open(CUSTOMERS_FILE, O_RDONLY);

    Customer *cust = (Customer *)malloc(sizeof(Customer));

    while (read(fd, cust, sizeof(Customer)) > 0)
    {
        if (strcmp(user, cust->username) == 0)
            return cust;
    }

    free(cust);

    cust = NULL;

    return cust;
}

int main()
{
    init_db();
    char user[30], pass[15];

    printf("Username: ");
    scanf("%s", user);

    printf("Password: ");
    scanf("%s", pass);

    Customer *cust = get_customer(user);

    if (cust == NULL)
    {
        perror("No such user exists");
        exit(EXIT_FAILURE);
    }

    if (strcmp(cust->password, pass) != 0)
    {
        perror("Invalid Username / Password");
        exit(EXIT_FAILURE);
    }

    switch (cust->role)
    {
    case USER:
        printf("Hello User\n");
        break;
    case EMPLOYEE:
        printf("Hello Employee\n");
        break;
    case MANAGER:
        printf("Hello Manager\n");
        break;
    default:
        printf("Invalid User\n");
    }

    free(cust);

    cust = NULL;
}