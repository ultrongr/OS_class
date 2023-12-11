#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// Declare semaphores
sem_t  sa2, sb1, sc1, sc2, sz, sy, sx;

// Global variables
int a1, a2, b1, b2, c1, c2, y, w, x, z;

// Thread functions
void* thread1_func(void* arg) {
    a1 = 10;

    a2 = 11;
    sem_post(&sa2);

    sem_wait(&sc1);
    y = a1 + c1;
    sem_post(&sy);

    sem_wait(&sx);
    printf("x: %d\n", x);

    return NULL;
}

void* thread2_func(void* arg) {
    b1 = 20;
    sem_post(&sb1);

    b2 = 21;

    sem_wait(&sc2);
    w = b2 + c2;

    sem_wait(&sy);
    sem_wait(&sz);
    x = z - y + w;
    sem_post(&sx);

    return NULL;
}

void* thread3_func(void* arg) {
    c1 = 30;
    sem_post(&sc1);

    c2 = 31;
    sem_post(&sc2);

    sem_wait(&sb1);
    sem_wait(&sa2);
    z = a2 + b1;
    sem_post(&sz);

    return NULL;
}

int main() {
    pthread_t t1, t2, t3;

    // Initialize semaphores
    sem_init(&sa2, 0, 0);
    sem_init(&sb1, 0, 0);
    sem_init(&sc1, 0, 0);
    sem_init(&sc2, 0, 0);
    sem_init(&sz, 0, 0);
    sem_init(&sy, 0, 0);
    sem_init(&sx, 0, 0);

    // Create threads
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);
    pthread_create(&t3, NULL, thread3_func, NULL);

    // Join threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    // Destroy semaphores
    sem_destroy(&sa2);
    sem_destroy(&sb1);
    sem_destroy(&sc1);
    sem_destroy(&sc2);
    sem_destroy(&sz);
    sem_destroy(&sy);
    sem_destroy(&sx);

    return 0;
}
