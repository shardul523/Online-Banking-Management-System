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

Bool update_record(int cust_c, int emp_c, int adm_c, int loan_c)
{
    Record rec = {cust_c, emp_c, adm_c, loan_c};

    int record_fd = open(RECORDS_FILE, O_WRONLY);

    if (record_fd == -1)
        return False;

    struct flock lock = set_lock(record_fd, SEEK_SET, 0, 0, 0);

    if (write(record_fd, &rec, sizeof(Record)) < 0)
    {
        unlock(record_fd, &lock);
        return False;
    }

    unlock(record_fd, &lock);

    return True;
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

char* join_words(char** words, int c) 
{
    char *sentence = (char*) calloc(FEEDBACK_SIZE, sizeof(char));
    
    for (int i = 1; i <= c; i++) {
        strcat(sentence, words[i]);
    } 

    return sentence;
}

#endif