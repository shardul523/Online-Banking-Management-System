#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "globals.h"

Customer *customer_login(int socket_fd, char *username, char *password);
void display_customer_menu();

Employee *employee_login(char *username, char *password);
void display_employee_menu();
void display_manager_menu();

Admin *admin_login(char *username, char *password);
void display_admin_menu();
/*
METHOD - GET

ACTION -
TYPE -
ARGUEMENTS -


TOKEN -
1. USER TYPE
2. USER ID
 */

// #include <stdio.h>

// #include "client_admin.c"
// #include "client_customer.c"
// #include "client_employee.c"

// int main()
// {
//     int user_type;
//     char username[USERNAME_SIZE], password[PASSWORD_SIZE];

//     printf("LOGIN\n");
//     printf("1. CUSTOMER\n");
//     printf("2. EMPLOYEE\n");
//     printf("3. ADMIN\n");

//     printf("Enter user type: ");
//     scanf("%d", &user_type);

//     printf("Username: ");
//     scanf("%s", username);
//     printf("Password: ");
//     scanf("%s", password);

//     switch (user_type)
//     {
//     case CUSTOMER:
//         customer_login(username, password);
//         display_customer_menu();
//         break;
//     case EMPLOYEE:
//         employee_login(username, password);
//         display_employee_menu();
//         break;
//     case ADMIN:
//         admin_login(username, password);
//         display_admin_menu();
//         break;
//     default:
//         break;
//     }
// }

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

// client.c
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// // Define the structured message
// struct Message
// {
//     int id;
//     char text[256];
// };

// #define PORT 8080
// #define SERVER_IP "127.0.0.1"

// int main()
// {
//     int sock = 0;
//     struct sockaddr_in serv_addr;

//     // Create a socket
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//     {
//         perror("Socket creation error");
//         exit(EXIT_FAILURE);
//     }

//     // Configure server address
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(PORT);

//     // Convert IP address from text to binary form
//     if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
//     {
//         perror("Invalid address/ Address not supported");
//         close(sock);
//         exit(EXIT_FAILURE);
//     }

//     // Connect to the server
//     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
//     {
//         perror("Connection failed");
//         close(sock);
//         exit(EXIT_FAILURE);
//     }

//     struct Message msg;

//     // Send and receive multiple messages
//     for (int i = 0; i < 3; i++)
//     {
//         // Prepare the message
//         msg.id = i;
//         snprintf(msg.text, sizeof(msg.text), "Client Message %d", i);

//         // Send the message to the server
//         send(sock, &msg, sizeof(msg), 0);

//         // Receive the modified message from the server
//         if (read(sock, &msg, sizeof(msg)) > 0)
//         {
//             printf("Received: [ID=%d] %s\n", msg.id, msg.text);
//         }
//     }

//     close(sock);
//     printf("Client terminated.\n");

//     return 0;
// }

int connect_server()
{
    struct sockaddr_in server_address;
    int socket_fd;

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (socket_fd < 0)
        return -1;

    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_family = AF_UNIX;
    server_address.sin_port = SERVER_PORT;

    if (connect(socket_fd, &server_address, sizeof(server_address)) < 0)
        return -1;

    return socket_fd;
}

int main()
{
    int socket_fd = connect_server();

    if (socket_fd == -1)
    {
        perror("Could not connect to the server");
        exit(EXIT_FAILURE);
    }

    int user_type = 0;
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
        customer_login(socket_fd, username, password);
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

// CUSTOMER FUNCTIONS

Customer *customer_login(int socket_fd, char *username, char *password)
{
}

void display_customer_menu()
{
    printf("1. View Account Balance\n");
    printf("2. Withdraw Money\n");
    printf("3. Deposit Money\n");
    printf("4. Transfer Funds\n");
    printf("5. View Transaction History\n");
    printf("6. Apply for a Loan\n");
    printf("7. Adding Feedback\n");
    printf("8. Change Password\n");
    printf("9. Logout\n");
}

Employee *employee_login(char *username, char *password)
{
    return 0;
}

void display_employee_menu()
{
    printf("1. Add New Customer\n");
    printf("2. Modify Customer Details\n");
    printf("3. Approve/Reject Loans\n");
    printf("4. View Assigned Loan Applications\n");
    printf("5. View Customer Transactions\n");
    printf("6. Change Password\n");
    printf("7. Logout\n");
}

void display_manager_menu()
{
    printf("1. Activate/Deactivate Customer Accounts\n");
    printf("2. Assign Loan Application Processes to Employees\n");
    printf("3. Review Customer Feedback\n");
    printf("4. Change Password\n");
    printf("5. Logout\n");
}

Admin *admin_login(char *username, char *password)
{
    return 0;
}

void display_admin_menu()
{
    printf("1. Add New Bank Employee\n");
    printf("2. Modify Customer/Employee Details\n");
    printf("3. Manage User Roles\n");
    printf("4. Change Password\n");
    printf("5. Logout\n");
}