//
// Created by Julien Debroize on 22/11/2016.
//

#include "Daemon.h"
#include "Client.h"

char *ANSWERPIPE = NULL; //The pipe for the daemon response
void closing(int signum); //This function is call in case of signal interruption for properly close the programm

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

    //open the second semaphore
    sem_t *sem_two = sem_open(SEMAPHORE_TWO, O_RDWR);

    //Check for errors
    if(sem_two == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /**ANSWERPIPE GENERATION PART**/
    //Create a pipe with pid name like /tmp/client12345
    //For simplification, we create a string name answerpipe and we copy it in ANSWERPIPE
    //We take the length of pid
    int sizeOfPid = sizeof(my_request.pid);
    int sizeOfPathPipe = SIZEOFPATH + sizeOfPid;
    //We convert the pid to a string
    char pidString[sizeOfPid];
    if(snprintf(pidString, sizeOfPid + 1, "%d", getpid()) == 0){
        perror("snprintf");
        exit(EXIT_FAILURE);
    }

    char answerPipe[sizeOfPathPipe];
    char *path = "/tmp/client";
    if(strcpy(answerPipe, path) == NULL){
        perror("strcpy");
        exit(EXIT_FAILURE);
    }

    if(strcat(answerPipe, pidString) == NULL){
        perror("strcat");
        exit(EXIT_FAILURE);
    }
    //answerPipe now equal /tmp/ClientPID, we copy it in ANSWERPIPE

    ANSWERPIPE = malloc(sizeof(answerPipe));//Memory allocation of ANSWERPIPE
    if(strcat(ANSWERPIPE, answerPipe) == NULL){
        perror("strcat");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", answerPipe);
    /**ANSWERPIPE GENERATION PART END**/

    /**PIPE CREATION PART**/
    //Create first pipe
    if(mkfifo(PIPE1, S_IRUSR | S_IWUSR) == - 1){
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    
    //Create answerPipe
    if(mkfifo(ANSWERPIPE, S_IRUSR | S_IWUSR) == -1){
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    /**PIPE GENERATION END**/
    //mapping of virtual address
    struct tabRequest *msq = mmap(NULL, SIZE_DATA, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (msq == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    //We block the semaphore and write the message
    if(sem_wait(sem_two) == -1){
        perror("sem_wait");
        exit(EXIT_FAILURE);

    }

    /*struct requete *stdRequete  = mmap(NULL, SIZE_DATA, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (msq == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }*/

    struct requete *stdRequete = argv[1], pid;
    /*stdRequete->pid = getpid();
    if(strcpy(stdRequete->cmd, argv[1]) == NULL){
        perror("strcpy \n");
        exit(EXIT_FAILURE);
    }*/

    msq->myRequest[0]->pid = stdRequete->pid;
    printf("%d\n", msq->myRequest[0]->pid);

    if(strcpy(msq->myRequest[0]->cmd, stdRequete->cmd) == NULL){
        perror("strcpy \n");
        exit(EXIT_FAILURE);
    }
    printf("le pid est %d et la commande est %s \n", msq->myRequest[0]->pid, msq->myRequest[0]->cmd);
    //We unlock the semaphore
    if(sem_post(sem_two) == -1){
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    if(sem_post(sem_p) == -1){
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
    //Open ANSWERPIPE in read mode
    int fdr = open(ANSWERPIPE, O_RDONLY);

    //Check for error in open
    if(fdr == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Read the message in ANSWERPIPE (the command return)
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

    //Close ANSWERPIPE
    if(unlink(ANSWERPIPE) == -1){
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    free(ANSWERPIPE); //No return control because free return nothing, see man for more details

    exit(EXIT_SUCCESS);
}

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

    //Close ANSWERPIPE
    if(unlink(ANSWERPIPE) == -1){
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    free(ANSWERPIPE); //No return control because free return nothing, see man for more details

    exit(EXIT_SUCCESS);
}