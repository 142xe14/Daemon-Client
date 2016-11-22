//
// Created by Julien Debroize on 22/11/2016.
//
//Wait for a client and launch command

#include "Daemon.h"

int main(){

    //Create first pipe
    if(mkfifo(PIPE1, S_IRUSR | S_IWUSR) == - 1){
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    //Create the second pipe
    if(mkfifo(TUBE2, S_IRUSR | S_IWUSR) == -1){
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    //Open the first pipe in read mode
    int fd = open(PIPE1, O_RDONLY);

    //Check for error in open
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Close pipe1
    if(unlink(PIPE1) == -1){
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    //Close tube2
    if(unlink(TUBE2) == -1){
        perror("unlink");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
