#include "globals.h"

typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} Admin;

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