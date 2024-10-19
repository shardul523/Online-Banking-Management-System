#include "common_client.c"

void display_admin_menu()
{
    printf("1. Add New Bank Employee\n");
    printf("2. Modify Customer/Employee Details\n");
    printf("3. Manage User Roles\n");
    printf("4. Change Password\n");
    printf("5. Logout\n");
}

void admin_handler(int sock_fd, Token* user)
{
    while (1) {
        int choice;

        printf("Hello, %s\n", user->username);
        display_admin_menu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("Add New Bank Employee\n");
                break;

            case 5:
                logout(sock_fd, user);
                return;
            
            default:
                return;
        }
    }
}