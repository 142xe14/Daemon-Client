//
// Created by Julien Debroize on 22/11/2016.
//

#include "Daemon.h"
#include "Client.h"

void closing(int signum) {
    if (signum < 0) {
        fprintf(stderr, "Wrong signal number\n");
        exit(EXIT_FAILURE);
    }
    printf("Stop by ctrl+c \n");
    //Close pipe1
    if (unlink(PIPE1) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    //Close tube2
    if (unlink(TUBE2) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
}

//Use Daemon for launch command
int main(int argc, char *argv[]){

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

    char returnCmd[SIZE_DATA];
    struct requete my_request;

    //Control usage of program
    if(argc < 2){
        printf("Usage Error! \n");
        exit(EXIT_FAILURE);
    }

    //Copy argv[1] in our struct request
    if(strcpy(my_request.cmd, argv[1]) == NULL){
        fprintf(stderr, "error strcpy \n");
        exit(EXIT_FAILURE);
    }

    //Copy programm PID in our struct request
    my_request.pid = getpid(); // No need to check for error, this function always work (see man for detail)

    //open the shm
    int shm_fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);

    //check for error
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    //open the semaphore
    sem_t *sem_p = sem_open(SEMAPHORE_NAME, O_RDWR);

    //Check for errors
    if(sem_p == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /*TODO
     * CREATE A PIPE WITH PID AS NAME LIKE /tmp/Client12345
    */

    int sizeOfPid = sizeof(my_request.pid); //On récupère la taille du PID
    int sizeOfPathPipe = SIZEOFPATH + sizeOfPid;

    char firstPipe[sizeOfPathPipe];
    
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

    //mapping of virtual address
    struct myshmstruct *msq = mmap(NULL, SIZE_DATA, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (msq == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if(strcpy(msq->buffer, argv[1]) == NULL){
        fprintf(stderr, "error strcpy \n");
        exit(EXIT_FAILURE);
    }

    if(sem_post(sem_p) == -1){
        perror("sem_post");
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