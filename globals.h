#ifndef GLOBALS_H
#define GLOBALS_H

#define CUSTOMERS_FILE "../data/CUSTOMERS_DB"
#define EMPLOYEES_FILE "../data/EMPLOYEES_DB"
#define ADMIN_FILE "../data/ADMIN_DB"
#define RECORDS_FILE "../data/RECORDS_DB"
#define LOANS_FILE "../data/LOANS_DB"
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 15
#define FEEDBACK_SIZE 300
#define RES_BODY_SIZE 1024
#define MAX_ARGUMENT_SIZE 1024
#define SERVER_PORT 8080

typedef enum
{
    False,
    True
} Bool;

typedef enum
{
    NONE,
    CUSTOMER,
    EMPLOYEE,
    ADMIN
} UserType;

typedef enum
{
    REGULAR,
    MANAGER
} Role;

typedef struct
{
    int user_id;
    UserType user_type;
    Role role;
    char username[USERNAME_SIZE];
} Token;

typedef struct
{
    int argc;
    Token user;
    char arguments[MAX_ARGUMENT_SIZE];
} Request;

typedef struct
{
    Token user;
    char body[RES_BODY_SIZE];
} Response;

typedef struct
{
    int customer_id;
    double balance;
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    Bool is_active;
    // char feedback[FEEDBACK_SIZE];
} Customer;

typedef struct
{
    Role role;
    int employee_id;
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} Employee;

typedef struct
{
    int admin_id;
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} Admin;

typedef enum
{
    PENDING = 1,
    IN_REVIEW,
    GRANTED
} LoanStatus;

typedef enum
{
    PERSONAL = 1,
    EDUCATION,
    BUSINESS
} LoanType;

typedef struct
{
    int loan_id;
    double amount;
    LoanType type;
    int customer_id;
    int employee_id;
    LoanStatus status;
} Loan;

typedef struct
{
    int customers_count;
    int employees_count;
    int admins_count;
    int loans_count;
} Record;

char *loan_type_names[] = {
    "", "PERSONAL", "EDUCATION", "BUSINESS"};

#endif