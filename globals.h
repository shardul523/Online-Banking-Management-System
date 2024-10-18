#ifndef GLOBALS_H
#define GLOBALS_H

#define CUSTOMERS_FILE "customers_db"
#define EMPLOYEES_FILE "employees_db"
#define ADMIN_FILE "admin_db"
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 15
#define FEEDBACK_SIZE 300
#define MAX_ARGUMENT_SIZE 100
#define SERVER_PORT 8080

typedef enum
{
    True,
    False
} Bool;

typedef enum
{
    NONE,
    CUSTOMER,
    EMPLOYEE,
    ADMIN
} UserType;

typedef struct
{
    int user_id;
    UserType user_type;
} Token;

typedef struct
{
    int action_id;
    int argc;
    char arguments[MAX_ARGUMENT_SIZE];
    Token user;
} Request;

typedef struct
{
    Customer c;
    Employee e;
    Admin m;
    Token user;
} Response;

typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    int customer_id;
    double balance;
    char feedback[FEEDBACK_SIZE];
} Customer;

typedef enum
{
    REGULAR,
    MANAGER
} Role;

typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    int employee_id;
    Role role;
} Employee;

typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} Admin;

#endif