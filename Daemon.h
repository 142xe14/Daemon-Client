//
// Created by Julien Debroize on 22/11/2016.
//

#ifndef SE_DAEMON_H_H
#define SE_DAEMON_H_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#define PIPE1 "/tmp/pipe1"
#define SIZE_DATA 256 //The maximum size of the command
#define SHM_NAME "/shm_daemon" //the partaged memory
#define SEMAPHORE_NAME "/my_sem_123456789098765"
#define SEMAPHORE_TWO "/my_sem_987654321012345"
#define MAX_REQUEST 100 //Maximum number of request
struct tabRequest{
    struct requete *myRequest[MAX_REQUEST];
};

struct requete{
    char cmd[SIZE_DATA];
    pid_t pid;
};

struct myshmstruct{
    char buffer[SIZE_DATA];
};

#endif //SE_DAEMON_H_H
