#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdbool.h>

bool ends = false;
int a = 0;

void launch(int signum) {
    if (a == 1)
        printf("Launch process!\n");
}

void end(int signum) {
    ends = true;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        exit(EXIT_FAILURE);
    } else {
        signal(SIGUSR1, launch);
    }

    int stat, stat2;
    key_t fkey1, fkey2;
    int sem1, sem2, res = 0, shm1, shm2, p1[2], p2[2];
    pid_t pid_1, pid_2, pid_pr, pid_pt, pid_ps, pid_pd, pid_ps1, pid_ps2;
    char pr1[32], pw1[32], pr2[32], pw2[32], pid_p1[32], pid_p2[32], pid_sem1[32], pid_sem2[32], pid_shm1[32], pid_shm2[32];

    if (pipe(p1) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(p2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    sprintf(pr1, "%d", p1[0]);
    sprintf(pw1, "%d", p1[1]);
    sprintf(pw2, "%d", p2[1]);

    //fork 1
    pid_1 = fork();
    if (pid_1 < 0) {
        fprintf(stderr, "Fork p2 is failed!");
        return 1;
    } else if (pid_1 == 0) {
        execl("./proc_p1", "proc_p1", pw1, "p1.txt", NULL);
        return 0;
    }

    pause();
    pid_2 = fork();

    if (pid_2 < 0) {
        fprintf(stderr, "Fork p2 is failed!");
        return 1;
    } else if (pid_2 == 0) {
        execl("./proc_p2", "proc_p2", pw1, "p2.txt", NULL);
        return 0;
    }

    pause();
    sprintf(pid_p1, "%d", (int) pid_1);
    sprintf(pid_p2, "%d", (int) pid_2);
    pid_pr = fork();

    if (pid_pr < 0) {
        fprintf(stderr, "Fork pr is failed!");
        return 1;
    } else if (pid_pr == 0) {
        execl("./proc_pr", "proc_pr", pid_p1, pid_p2, pr1, pw2, NULL);
        return 0;
    }

    waitpid(pid_pr, &stat, WUNTRACED);
    kill(pid_1, SIGTERM);
    kill(pid_2, SIGTERM);
    signal(SIGTERM, end);
    fkey1 = ftok(".", 1);
    fkey2 = ftok(".", 2);
    shm1 = shmget(fkey1, 1024, 0660 | IPC_CREAT);

    if (shm1 == -1) {
        exit(EXIT_FAILURE);
    }

    shm2 = shmget(fkey2, 1024, 0660 | IPC_CREAT);
    if (shm2 == -1) {
        exit(EXIT_FAILURE);
    }

    sem1 = semget(fkey1, 2, 0660 | IPC_CREAT);
    if (sem1 == -1) {
        exit(EXIT_FAILURE);
    }

    sem2 = semget(fkey2, 2, 0660 | IPC_CREAT);
    if (sem2 == -1) {
        exit(EXIT_FAILURE);
    }

    //Inicialization semafor
    res = semctl(sem1, 0, SETVAL, 1);
    if (res == -1) {
        exit(EXIT_FAILURE);
    }

    res = semctl(sem1, 1, SETVAL, 0);
    if (res == -1) {
        exit(EXIT_FAILURE);
    }

    res = semctl(sem2, 0, SETVAL, 1);
    if (res == -1) {
        exit(EXIT_FAILURE);
    }

    res = semctl(sem2, 1, SETVAL, 0);
    if (res == -1) {
        exit(EXIT_FAILURE);
    }

    sprintf(pr2, "%d", p2[0]);
    sprintf(pid_sem1, "%d", sem1);
    sprintf(pid_shm1, "%d", shm1);
    sprintf(pid_sem2, "%d", sem2);
    sprintf(pid_shm2, "%d", shm2);

    pid_ps1 = fork();

    if (pid_ps1 < 0) {
        fprintf(stderr, "Fork proc_serv1 is failed!");
        return 1;
    } else if (pid_ps1 == 0) {
        execl("./proc_serv1", "proc_serv1", argv[1], argv[2], NULL);
        return 0;
    }

    //pause();
    pid_ps2 = fork();

    if (pid_ps2 < 0) {
        fprintf(stderr, "Fork proc_serv2 is failed!");
        return 1;
    } else if (pid_ps2 == 0) {
        execl("./proc_serv2", "proc_serv2", argv[2], NULL);
        return 0;
    }

    pid_pt = fork();
    if (pid_pt < 0) {
        fprintf(stderr, "Fork proc_t is failed!");
        return 1;
    } else if (pid_pt == 0) {
        execl("./proc_t", "proc_t", pr2, pid_shm1, pid_sem1, NULL);
        return 0;
    }

    pid_ps = fork();

    if (pid_ps < 0) {
        fprintf(stderr, "Fork proc_s is failed!");
        return 1;
    } else if (pid_ps == 0) {
        execl("./proc_s", "proc_s", pid_shm1, pid_sem1, pid_shm2, pid_sem2, NULL);
        return 0;
    }

    pid_pd = fork();
    a++;

    if (pid_pd < 0) {
        fprintf(stderr, "Fork proc_s is failed!");
        return 1;
    } else if (pid_pd == 0) {
        execl("./proc_d", "proc_d", pid_shm2, pid_sem2, argv[1], NULL);
        return 0;
    }

    while (!ends) {
        sleep(2);
    }

    kill(pid_pt, SIGTERM);
    kill(pid_ps, SIGTERM);
    kill(pid_pd, SIGTERM);
    sleep(1);
    semctl(sem1, 0, IPC_RMID);
    semctl(sem2, 0, IPC_RMID);

    shmctl(shm1, IPC_RMID, NULL);
    shmctl(shm2, IPC_RMID, NULL);

    close(atoi(argv[2]));
    close(atoi(argv[1]));
    printf("End of program.\n");
    exit(EXIT_SUCCESS);

    exit(EXIT_SUCCESS);
}
