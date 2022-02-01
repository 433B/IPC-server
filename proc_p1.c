#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

FILE* fd = NULL;
FILE* fderr = NULL;
int write_pipe = 0;

void sig_handler(int signum) {
    if (signum == SIGUSR1) {
        char buff[1024];
        if (fgets(buff, 1024, fd) == NULL) {
            exit(EXIT_SUCCESS);
        }

        int size = strlen(buff);
        int written = 0;
        char* buf = buff;

        while(size > 0) {
            written = write(write_pipe, buf, size);
            if (written == -1) {
                break;
            }
            buf += written;
            size -= written;
        }
    }
}

void end_of_proc(int signum){
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {

    write_pipe = atoi(argv[1]);
    fd = fopen("p1.txt", "r");

    if (fd == NULL) {
        printf("Error: file is not exist!");
        return -1;
    }

    signal(SIGUSR1, sig_handler);
    signal(SIGTERM, end_of_proc);
    if (kill(getppid(), SIGUSR1) < 0) {
        exit(EXIT_FAILURE);
    }

    for (;;) {
        sleep(1);
    }

    fclose(fd);
    return 0;
}