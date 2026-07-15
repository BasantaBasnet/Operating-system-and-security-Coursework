#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// 1. Process creation

void process_task(const char *name) {
    printf("[%s] started (PID=%d)\n", name, getpid());
    for (int step = 1; step <= 3; step++) {
        printf("[%s] working... step %d\n", name, step);
        usleep(500000);
    }
    printf("[%s] finished\n", name);
}

void run_process_creation(void) {
    printf("\n1. PROCESS CREATION\n");
    

    char names[3][16] = {"Process-1", "Process-2", "Process-3"};
    pid_t pids[3];

    for (int i = 0; i < 3; i++) {
        fflush(stdout);  // flush before fork so the child doesn't reprint buffered text
        pid_t pid = fork();
        if (pid == 0) {
            process_task(names[i]);
            exit(0);
        } else if (pid > 0) {
            pids[i] = pid;
        } else {
            perror("fork failed");
            exit(1);
        }
    }

    for (int i = 0; i < 3; i++) {
        waitpid(pids[i], NULL, 0);
    }

    printf("All processes completed\n");
}

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);

    run_process_creation();

    return 0;
}
