#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void end_of_proc(int signum){
    exit(EXIT_SUCCESS);
}

int main (int argc, char* argv[]){

    char *buffer;
    buffer = shmat(atoi(argv[1]), NULL, 0);

    kill(getppid(), SIGUSR1);
    signal(SIGTERM, end_of_proc);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    if (sockfd == -1) {
        perror("ERROR socket"); //vytvorenie socketu
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[3]));
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr*) &addr, sizeof (addr)) == -1) {
        perror("ERROR connecting");
        exit(1);
    }

    struct sembuf sembuff[1];
    for (;;) {
        sembuff[0].sem_num = 1;
        sembuff[0].sem_op = -1;
        sembuff[0].sem_flg = 0;
        semop(atoi(argv[2]),sembuff,1);
        int len = strlen(buffer);
        write(sockfd, buffer, len + 1);
        sleep(1);
        sembuff[0].sem_num = 0;
        sembuff[0].sem_op = 1;
        sembuff[0].sem_flg = 0;
        semop(atoi(argv[2]),sembuff,1);
    }
    
}
