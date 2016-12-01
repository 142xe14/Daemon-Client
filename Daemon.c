//
// Created by Julien Debroize on 22/11/2016.
//
//Wait for a client and launch command

#include "Daemon.h"


int main(){

    char cmd[SIZE_DATA];

    //We create our semaphore
    // On commence par créer notre sémaphore.
    sem_t *sem_p = sem_open(SEMAPHORE_NAME, O_RDWR | O_CREAT | O_EXCL,
                            S_IRUSR | S_IWUSR, 0);

    //Check for error
    if (sem_p == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // We create our shm
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    //check for error
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    //Define size of shm with ftruncate
    if (ftruncate(shm_fd, SIZE_DATA) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    //Wait for data write in shm
    sem_wait(sem_p);

    //Open the first pipe in read mode
    int fd = open(PIPE1, O_RDONLY);

    //Check for error in open
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Read the message in pipe1
    if(read(fd, cmd, SIZE_DATA) == -1){
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("(TEST) We read this message : %s \n", cmd);

    //Open the second pipe in write mode
    int fdw = open(TUBE2, O_WRONLY);

    //Check for error in open
    if (fdw == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Write the cmd in Tube2
    if(write(fdw, cmd, SIZE_DATA) == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }

    //Close our semaphore
    if (sem_unlink(SEMAPHORE_NAME) == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

    //Close shm
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
