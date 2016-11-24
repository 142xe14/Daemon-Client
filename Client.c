//
// Created by Julien Debroize on 22/11/2016.
//

#include "Daemon.h"

//Use Daemon for launch command
int main(int argc, char *argv[]){

    char returnCmd[SIZE_DATA];

    if(argc != 2){
        printf("Usage Error! \n");
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

    exit(EXIT_SUCCESS);
}