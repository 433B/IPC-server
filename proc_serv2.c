#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {
    struct sockaddr_in server_addr;
    char buff[256];
    int portno, sockfd, outfile;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    outfile = open("serv2.txt", O_CREAT | O_WRONLY, 0666);

    if (outfile < 0) {
        perror("Error with opening file\n");
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    portno = atoi(argv[1]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("ERROR on binding");
    }

    kill(getppid(), SIGUSR1);
    int i = 0;
    for (i = 0; i < 10; i++) {
        recv(sockfd, buff, 200, 0);
        write(outfile, buff, strlen(buff));
        write(outfile, "\n", 1);
    }

    kill(getppid(), SIGTERM);

    printf("Turn Off server2\n");
    return 0;
}