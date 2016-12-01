//
// Created by Julien Debroize on 22/11/2016.
//

#include "Daemon.h"

//Use Daemon for launch command
int main(int argc, char *argv[]){

    char returnCmd[SIZE_DATA];
    struct requete my_request;

    //Control usage of program
    if(argc != 2){
        printf("Usage Error! \n");
        exit(EXIT_FAILURE);
    }

    //Copy argv[1] in our struct request
    if(strcpy(my_request.cmd, argv[1]) == NULL){
        fprintf(stderr, "error strcpy \n");
        exit(EXIT_FAILURE);
    }

    //open the semaphore
    sem_t *sem_p = sem_open(SEMAPHORE_NAME, O_RDWR);
    //Check for errors
    if(sem_p == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

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



    //Open pipe1 in Write mode
    int fd = open(PIPE1, O_WRONLY);

    //Check for error in open
    if(fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("%s \n", argv[1]);
    //write argv[1] in the pipe
    if(write(fd, argv[1], SIZE_DATA) == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }

    //Open tube2 in read mode
    int fdr = open(TUBE2, O_RDONLY);

    //Check for error in open
    if(fdr == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Read the message in tube2 (the command return)
    if(read(fdr, returnCmd, SIZE_DATA) == -1){
        perror("read");
        exit(EXIT_FAILURE);
    }

    //Print the command return
    printf("%s \n", returnCmd);

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