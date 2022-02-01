#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>

void end_of_proc(int signum){
    exit(EXIT_SUCCESS);
}

int main (int argc, char* argv[]) {
    struct sembuf sem_b[1];
    int pipe, portno, semafor, a = 0;
    char *buffer;
    char string;
    char letters[128];

    kill(getppid(), SIGUSR1);
    signal(SIGTERM, end_of_proc);

    pipe = atoi(argv[1]);
    portno = atoi(argv[2]);
    semafor = atoi(argv[3]);
    buffer = shmat(portno, NULL, 0);
    if (buffer == NULL) {
        exit(EXIT_FAILURE);
    }

    do {
        sem_b[0].sem_num = 0;
        sem_b[0].sem_op = -1;
        sem_b[0].sem_flg = 0;

        if (semop(semafor, sem_b, 1)) {
            return (0);
        }
        do {
            read(pipe, &letters[a], 1);
            a++;
        } while (letters[a - 1] != '\n');

        letters[a - 1] = '\0';
        strcpy(buffer, letters);
        a = 0;

        sem_b[0].sem_num = 1;
        sem_b[0].sem_op = 1;
        sem_b[0].sem_flg = 0;

        if (semop(semafor, sem_b, 1)) {
            return (0);
        }
    } while (1);

    return 0;
}