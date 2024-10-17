#include <stdio.h>

#include "client_admin.c"
#include "client_customer.c"
#include "client_employee.c"

int main()
{
    int user_type;
    char username[USERNAME_SIZE], password[PASSWORD_SIZE];

    printf("LOGIN\n");
    printf("1. CUSTOMER\n");
    printf("2. EMPLOYEE\n");
    printf("3. ADMIN\n");

    printf("Enter user type: ");
    scanf("%d", &user_type);

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    switch (user_type)
    {
    case CUSTOMER:
        customer_login(username, password);
        display_customer_menu();
        break;
    case EMPLOYEE:
        employee_login(username, password);
        display_employee_menu();
        break;
    case ADMIN:
        admin_login(username, password);
        display_admin_menu();
        break;
    default:
        break;
    }
}

// typedef struct
// {
//     long long acc_no;
//     char username[30];
//     char password[15];
// } Customer;

// void init_db()
// {
//     int fd = open(CUSTOMERS_FILE, O_WRONLY);
//     Customer customers[] = {
//         {1, "shardul05", "test1234"},
//         {2, "sanal", "test1234"},
//         {3, "alka158", "test1234"}};
//     write(fd, customers, sizeof(customers));
//     close(fd);
// }

// Customer *get_customer(char *user)
// {
//     int fd = open(CUSTOMERS_FILE, O_RDONLY);

//     Customer *cust = (Customer *)malloc(sizeof(Customer));

//     while (read(fd, cust, sizeof(Customer)) > 0)
//     {
//         if (strcmp(user, cust->username) == 0)
//             return cust;
//     }

//     free(cust);

//     cust = NULL;

//     return cust;
// }

// int main()
// {
//     init_db();
//     char user[30], pass[15];

//     printf("Username: ");
//     scanf("%s", user);

//     printf("Password: ");
//     scanf("%s", pass);

//     Customer *cust = get_customer(user);

//     if (cust == NULL)
//     {
//         perror("No such user exists");
//         exit(EXIT_FAILURE);
//     }

//     if (strcmp(cust->password, pass) != 0)
//     {
//         perror("Invalid Username / Password");
//         exit(EXIT_FAILURE);
//     }

//     switch (cust->role)
//     {
//     case USER:
//         printf("Hello User\n");
//         break;
//     case EMPLOYEE:
//         printf("Hello Employee\n");
//         break;
//     case MANAGER:
//         printf("Hello Manager\n");
//         break;
//     default:
//         printf("Invalid User\n");
//     }

//     free(cust);

//     cust = NULL;
// }