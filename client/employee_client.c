#include "common_client.c"

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

void regular_employee_handler(int sock_fd, Token* user)
{
    while (1) {
        int choice;

        printf("Hello, %s\n", user->username);
        display_employee_menu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("Adding New Customer\n");
                break;

            case 7:
                logout(sock_fd, user);
                return;
            
            default:
                return;
        }
    }
}

void manager_employee_handler(int sock_fd, Token *user)
{
    while (1) {
        int choice;

        printf("Hello, %s\n", user->username);
        display_manager_menu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("Activate / Deactivate Customer Accounts\n");
                break;

            case 5:
                logout(sock_fd, user);
                return;
            
            default:
                return;
        }
    }
}


void employee_handler(int sock_fd, Token* user) 
{
    if (user->role == REGULAR) regular_employee_handler(sock_fd, user);
    else if (user->role == MANAGER) manager_employee_handler(sock_fd, user);
}

