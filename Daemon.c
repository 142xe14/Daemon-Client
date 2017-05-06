//
// Created by Julien Debroize on 22/11/2016.
//
//Wait for a client and launch command

//Daemon need to launch more than one command
#include "Daemon.h"

void closing(int signum) {
    if (signum < 0) {
        fprintf(stderr, "Wrong signal number\n");
        exit(EXIT_FAILURE);
    }
    printf("Daemon will properly be close \n");
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

void *cmd(void *data){

    struct myshmstruct *msq = data;

    //Open the first pipe in write mode
    int fd = open(TUBE2, O_WRONLY);

    //Check for error in open
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if(dup2(fd, STDOUT_FILENO) == -1){
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    if(system(msq->buffer) == -1){
        perror("System");
        exit(EXIT_FAILURE);
    }

    pthread_exit(NULL);
}

int main(){

    //For signal
    struct sigaction action;
    action.sa_handler = closing;
    action.sa_flags = 0;
    if (sigfillset(&action.sa_mask) == -1) {
        perror("sigfilltset");
        exit(EXIT_FAILURE);
    }

    // Ctrl+c
    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    pthread_t tr;
    //We create our semaphore
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

    //mapping of virtual address
    struct myshmstruct *msq = mmap(NULL, SIZE_DATA, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (msq == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    while(1) {
        //Wait for data write in shm
        if (sem_wait(sem_p) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        //Launch the command in a new thread
        if (pthread_create(&tr, NULL, cmd, msq) == -1) {
            perror("pthread");
            exit(EXIT_FAILURE);
        }
    }

    printf("You can't see this message! \n");
    exit(EXIT_SUCCESS);
}
