#include <stdio.h>
#include <string.h>

#include "../globals.h"

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

int view_account_balance(int sock_fd, Token* user);

void customer_handler(int sock_fd, Token *user) 
{
    int choice;
    
    while (1) {
        display_customer_menu();
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                //printf("Get customer balance\n");
                view_account_balance(sock_fd, user);
                break;

            case 2:
                printf("Withdraw money from account\n");
                break;

            case 3:
                printf("Deposit money in customer's account\n");
                break;

            case 4:
                printf("Transfer funds to another account\n");
                break;
            
            default:
                printf("Operation Not Found\n");
                break;
        }
    }
}


int view_account_balance(int sock_fd, Token* user)
{
    Request req;
    Response res;

    req.user = *user;
    req.argc = 1;
    strcpy(req.arguments, "GET_BALANCE");

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) 
        return -1;
    
    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return -1;

    printf("%s\n", res.body);

    return 0;
}