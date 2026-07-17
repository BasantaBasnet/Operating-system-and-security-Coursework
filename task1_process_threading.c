#define _GNU_SOURCE
#include <pthread.h>
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


//  2. Threads 

void *thread_task(void *arg) {
    char *name = (char *)arg;
    for (int i = 1; i <= 4; i++) {
        printf("[%s] running iteration %d\n", name, i);
        usleep(300000);
    }
    printf("[%s] done\n", name);
    return NULL;
}

void run_multithreading(void) {
    printf("\n2. MULTI-THREADING (3 threads)\n");

    pthread_t threads[3];
    char names[3][16] = {"Thread-1", "Thread-2", "Thread-3"};

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_task, names[i]);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads completed\n");
}
// 3. Race condition

#define ITERATIONS 100000

long counter = 0;
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;


void *increment_unsafe(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        counter++;
    }
    return NULL;
}

void *increment_safe(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&counter_lock);
        counter++;
        pthread_mutex_unlock(&counter_lock);
    }
    return NULL;
}

void run_race_condition(void) {
    printf("\n3. RACE CONDITION DEMO\n");

    pthread_t threads[3];

    counter = 0;
    for (int i = 0; i < 3; i++) pthread_create(&threads[i], NULL, increment_unsafe, NULL);
    for (int i = 0; i < 3; i++) pthread_join(threads[i], NULL);
    printf("Without mutex: Expected %d, got %ld (RACE CONDITION!)\n", 3 * ITERATIONS, counter);

    counter = 0;
    for (int i = 0; i < 3; i++) pthread_create(&threads[i], NULL, increment_safe, NULL);
    for (int i = 0; i < 3; i++) pthread_join(threads[i], NULL);
    printf("With mutex: Expected %d, got %ld (CORRECT)\n", 3 * ITERATIONS, counter);
}

//   main

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);  // line-buffer output so fork() doesn't >

    printf("TASK 1: PROCESS MANAGEMENT AND THREADING\n");

    run_process_creation();
    run_multithreading();
    run_race_condition();
    //run_semaphore();
   // run_scheduler();
   // run_deadlock();

   // printf("ALL DEMONSTRATIONS COMPLETED\n");
   // printf("  1. Process creation: 3 child processes (fork)\n");
   // printf("  2. Multithreading: 3 threads (pthreads)\n");
    //printf("  3. Mutex: race condition fixed\n");
   // printf("  4. Semaphore: connection pool with 2 concurrent\n");
    //printf("  5. Round robin scheduler: 4 processes simulated\n");
   // printf("  6. Deadlock: shown and then prevented\n");

    return 0;

}
