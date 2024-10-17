#include "globals.h"

typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    int customer_id;
    double balance;
    char feedback[FEEDBACK_SIZE];
} Customer;

Customer *customer_login(char *username, char *password)
{
    return 0;
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