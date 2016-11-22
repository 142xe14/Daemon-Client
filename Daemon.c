//
// Created by Julien Debroize on 22/11/2016.
//
//Wait for a client and launch command

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
    int fd = open(TUBE_NOM, O_RDONLY);

    //Check for error in open
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

}
