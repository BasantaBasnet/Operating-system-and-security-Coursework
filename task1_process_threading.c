#define _GNU_SOURCE
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
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

// 4. Semaphore

sem_t connection_pool;

void *use_connection(void *arg) {
    char *name = (char *)arg;
    printf("[%s] waiting for a connection...\n", name);

    sem_wait(&connection_pool);  // only 2 threads get past this at once
    printf("[%s] got a connection, working...\n", name);
    usleep(600000);
    sem_post(&connection_pool);  // free up a slot for someone else

    printf("[%s] released the connection\n", name);
    return NULL;
}

void run_semaphore(void) {
    printf("\n4. SEMAPHORE DEMO (max 2 concurrent)\n");

    sem_init(&connection_pool, 0, 2);

    pthread_t threads[4];
    char names[4][16] = {"Client-1", "Client-2", "Client-3", "Client-4"};

    for (int i = 0; i < 4; i++) pthread_create(&threads[i], NULL, use_connection, names[i]);
    for (int i = 0; i < 4; i++) pthread_join(threads[i], NULL);

    sem_destroy(&connection_pool);
    printf("Semaphore demonstration completed\n");
}

// 5. Round robin scheduler

typedef struct {
    char name[16];
    int burst_time;
    int remaining;
    int waiting_time;
    int completion_time;
    int turnaround_time;
} SchedProcess;

void round_robin(SchedProcess procs[], int n, int quantum) {
    printf("\n--- Round Robin (quantum = %d) ---\n", quantum);
    printf("%-8s %-12s %-20s\n", "Time", "Process", "Action");

    int queue[100], front = 0, back = 0, qsize = 0;
    for (int i = 0; i < n; i++) { queue[back++] = i; qsize++; }

    int clock_time = 0;

    while (qsize > 0) {
        int idx = queue[front];
        front = (front + 1) % 100;
        qsize--;

        SchedProcess *p = &procs[idx];
        int run_time = (p->remaining < quantum) ? p->remaining : quantum;

        printf("%-8d %-12s Running for %d units\n", clock_time, p->name, run_time);

        p->remaining -= run_time;
        clock_time += run_time;

        if (p->remaining > 0) {
            queue[back] = idx;
            back = (back + 1) % 100;
            qsize++;
        } else {
            p->completion_time = clock_time;
            p->turnaround_time = p->completion_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            printf("%-8d %-12s COMPLETED\n", clock_time, p->name);
        }
    }

    printf("\n Stats\n");
    double total_wait = 0, total_turn = 0;
    for (int i = 0; i < n; i++) {
        printf("%s: Burst=%d, Waiting=%d, Turnaround=%d\n",
               procs[i].name, procs[i].burst_time, procs[i].waiting_time, procs[i].turnaround_time);
        total_wait += procs[i].waiting_time;
        total_turn += procs[i].turnaround_time;
    }
    printf("Average Waiting Time: %.2f\n", total_wait / n);
    printf("Average Turnaround Time: %.2f\n", total_turn / n);
}

void run_scheduler(void) {
    printf("\n5. ROUND ROBIN SCHEDULER\n");
    printf("\n");

    SchedProcess procs[4] = {
        {"P1", 5, 5, 0, 0, 0},
        {"P2", 3, 3, 0, 0, 0},
        {"P3", 7, 7, 0, 0, 0},
        {"P4", 2, 2, 0, 0, 0}
    };

    round_robin(procs, 4, 2);
}

//  6. Deadlock 

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    char name[16];
    pthread_mutex_t *first;
    pthread_mutex_t *second;
} LockArgs;

// This can really deadlock if two threads lock in opposite order.
// A timeout stops the program from freezing so we can see it happen.
void *deadlock_causing(void *arg) {
    LockArgs *a = (LockArgs *)arg;

    printf("[%s] trying to acquire first lock\n", a->name);
    pthread_mutex_lock(a->first);
    printf("[%s] ACQUIRED first lock\n", a->name);
    usleep(200000);

    printf("[%s] trying to acquire second lock\n", a->name);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;

    int rc = pthread_mutex_timedlock(a->second, &ts);
    if (rc == 0) {
        printf("[%s] ACQUIRED second lock\n", a->name);
        pthread_mutex_unlock(a->second);
    } else {
        printf("[%s] COULD NOT acquire second lock (DEADLOCK AVOIDED by timeout)\n", a->name);
    }

    pthread_mutex_unlock(a->first);
    printf("[%s] released locks\n", a->name);
    return NULL;
}

// Every thread locks the resources in the same order (lock_a then lock_b).
// This avoids the circular wait that caused the deadlock above.
void *deadlock_safe(void *arg) {
    LockArgs *a = (LockArgs *)arg;

    pthread_mutex_t *ordered_first  = (a->first < a->second) ? a->first : a->second;
    pthread_mutex_t *ordered_second = (a->first < a->second) ? a->second : a->first;

    printf("[%s] trying to acquire locks in consistent order\n", a->name);
    pthread_mutex_lock(ordered_first);
    printf("[%s] acquired first lock\n", a->name);
    usleep(100000);

    pthread_mutex_lock(ordered_second);
    printf("[%s] acquired second lock\n", a->name);
    usleep(100000);
    printf("[%s] safely holding both locks\n", a->name);

    pthread_mutex_unlock(ordered_second);
    pthread_mutex_unlock(ordered_first);
    printf("[%s] released all locks\n", a->name);
    return NULL;
}

void run_deadlock(void) {
    printf("\n6. DEADLOCK DEMO AND PREVENTION\n");


    printf("\n Unsafe order (can deadlock) \n");

    pthread_t t1, t2;
    LockArgs argsA = {"Thread-A", &lock_a, &lock_b};
    LockArgs argsB = {"Thread-B", &lock_b, &lock_a};

    pthread_create(&t1, NULL, deadlock_causing, &argsA);
    pthread_create(&t2, NULL, deadlock_causing, &argsB);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n Safe order (consistent locking) \n");

    pthread_create(&t1, NULL, deadlock_safe, &argsA);
    pthread_create(&t2, NULL, deadlock_safe, &argsB);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nDeadlock prevention successful - all threads completed\n");
}

//   main

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);  // line-buffer output so fork() doesn't >

    printf("TASK 1: PROCESS MANAGEMENT AND THREADING\n");

    run_process_creation();
    run_multithreading();
    run_race_condition();
    run_semaphore();
    run_scheduler();
    run_deadlock();

    printf("ALL DEMONSTRATIONS COMPLETED\n");
    printf("  1. Process creation: 3 child processes (fork)\n");
    printf("  2. Multithreading: 3 threads (pthreads)\n");
    printf("  3. Mutex: race condition fixed\n");
    printf("  4. Semaphore: connection pool with 2 concurrent\n");
    printf("  5. Round robin scheduler: 4 processes simulated\n");
    printf("  6. Deadlock: shown and then prevented\n");

    return 0;

}
