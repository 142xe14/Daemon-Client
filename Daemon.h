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
#define TUBE2 "/tmp/tube2"
#define SIZE_DATA 256 //The maximum size of the command
#define SHM_NAME "/shm_daemon" //the partaged memory
#define SEMAPHORE_NAME "/my_sem_123456789098765"

struct requete{
    char cmd[SIZE_DATA];
    pid_t pid;
};

struct myshmstruct{
    char buffer[SIZE_DATA];
};

#endif //SE_DAEMON_H_H
