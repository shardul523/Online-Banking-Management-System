#ifndef COMMON_SERVER_H
#define COMMON_SERVER_H

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "../globals.h"

struct flock set_lock(int fd, short whence, int start, int len, int type)
{
    struct flock lock;
    lock.l_type = type == 1 ? F_RDLCK : F_WRLCK;
    lock.l_whence = whence;
    lock.l_start = start;
    lock.l_len = len;
    lock.l_pid = getpid();

    fcntl(fd, F_SETLKW, &lock);

    return lock;
}

int unlock(int fd, struct flock *lock)
{
    return fcntl(fd, F_SETLK, lock);
}

void logout(Response *res)
{
    res->user.user_id = -1;
    res->user.user_type = NONE;
    strcpy(res->user.username, "");
    strcpy(res->body, "Logout successful\n");
}


// RECORD FUNCTIONS

Record *get_record()
{
    int record_fd = open(RECORDS_FILE, O_RDONLY);

    if (record_fd == -1)
        return NULL;

    Record *rec = (Record *)malloc(sizeof(Record));

    struct flock lock;

    lock = set_lock(record_fd, SEEK_SET, 0, 0, 1);

    if (read(record_fd, rec, sizeof(Record)) < 0)
    {
        unlock(record_fd, &lock);
        return NULL;
    }

    unlock(record_fd, &lock);

    return rec;
}

Bool update_record(Record *rec)
{
    int record_fd = open(RECORDS_FILE, O_WRONLY);

    if (record_fd == -1)
        return False;

    struct flock lock = set_lock(record_fd, SEEK_SET, 0, 0, 0);

    if (write(record_fd, rec, sizeof(Record)) < 0)
    {
        unlock(record_fd, &lock);
        return False;
    }

    unlock(record_fd, &lock);

    return True;
}

// UTILITY FUNCTIONS

void extract_args(int c, char **a, char *str)
{
    char delim[] = " ";
    char *saveptr;

    a[0] = __strtok_r(str, delim, &saveptr);

    for (int i = 1; i < c; i++)
    {
        a[i] = __strtok_r(NULL, delim, &saveptr);
    }
}


Bool are_equal(char *str1, char *str2)
{
    return strcmp(str1, str2) == 0;
}

void print_customer(Customer *cust)
{
    printf("\nCustomer ID: %d\n", cust->customer_id);
    printf("Customer Username: %s\n", cust->username);
    printf("Is Customer Active: %d\n", cust->is_active);
}

char* join_words(char** words, int c) 
{
    char *sentence = (char*) calloc(FEEDBACK_SIZE, sizeof(char));
    
    for (int i = 1; i <= c; i++) {
        strcat(sentence, words[i]);
        strcat(sentence, " ");
    } 

    strcat(sentence, "\n");

    return sentence;
}

// LOAN FUNCTIONS

int get_loan(int loan_id, Loan *loan)
{
    int fd = open(LOANS_FILE, O_RDONLY);

    if (fd == -1)
        return -1;

    lseek(fd, (loan_id - 1) * sizeof(Loan), SEEK_SET);

    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Loan), 1);

    if (read(fd, loan, sizeof(Loan)) <= 0)
    {
        unlock(fd, &lock);
        return -1;
    }

    unlock(fd, &lock);

    return 0;
}

int update_loan(Loan *loan)
{
    int fd = open(LOANS_FILE, O_WRONLY);

    if (fd == -1)
        return -1;

    lseek(fd, (loan->loan_id - 1) * sizeof(Loan), SEEK_SET);
    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Loan), 0);

    if (write(fd, loan, sizeof(Loan)) <= 0)
    {
        unlock(fd, &lock);
        return -1;
    }

    unlock(fd, &lock);

    return 0;
}

void get_date(time_t t, char* date) {
    struct tm tm = *localtime(&t);

    snprintf(date, 49, "%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1990, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// TRANSACTION FUNCTIONS

int add_transaction(int sender_id, int receiver_id, double amount)
{
    int fd = open(TRANSACTIONS_FILE, O_APPEND | O_WRONLY | O_CREAT, 0666);

    if (fd == -1) return -1;

    Record* record = get_record();

    if (record == NULL) return -1;

    record->transactions_count++;

    Transaction new_transaction = {record->transactions_count, sender_id, receiver_id, amount, time(NULL)};

    if (update_record(record) == -1) return -1;

    struct flock lock = set_lock(fd, SEEK_SET, 0, 0, 1);
    
    if (write(fd, &new_transaction, sizeof(Transaction)) <= 0) {
        record->transactions_count--;
        update_record(record);
        unlock(fd, &lock);
        return -1;
    }

    unlock(fd, &lock);

    return 0;
}



// Transaction_id Date Sender_id Receiver_id Amount Type
void send_transactions_by_id(int customer_id, Response *res)
{
    Transaction transaction;

    int fd = open(TRANSACTIONS_FILE, O_RDONLY);

    snprintf(res->body, 4999, "%s | %-20s | %s | %s | %s | TYPE\n", 
    "TRANSACTION ID", "DATE", "SENDER ID", "RECEIVER ID", "AMOUNT");


    char curr_line[500];
    char curr_date[50];
    char curr_type[10];

    if (fd == -1) {
        strcpy(res->body, "Could not get your transactions\n");
        return;
    }

    while (read(fd, &transaction, sizeof(transaction)) > 0) {
        if (customer_id != transaction.receiver_id && customer_id != transaction.sender_id)
            continue;
        
        if (transaction.receiver_id != -1 && transaction.sender_id != -1)
            strcpy(curr_type, "TRANSFER");
        else if (transaction.receiver_id == customer_id)
            strcpy(curr_type, "DEPOSIT");
        else if (transaction.sender_id == customer_id)
            strcpy(curr_type, "WITHDRAW");

        get_date(transaction.time, curr_date);
        
        snprintf(curr_line, 499, "%-15d | %s | %-9d | %-11d | %.2lf | %s\n",
        transaction.transaction_id, curr_date, transaction.sender_id, transaction.receiver_id,
        transaction.amount, curr_type);
        
        strcat(res->body, curr_line);
    }

}

#endif