#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Declare semaphores
sem_t  sa2b1, sc1_x, sc2, szy;

// Global variables
int a1, a2, b1, b2, c1, c2, y, w, x, z;

// Thread functions
void* thread1_func(void* arg) {
    a1 = 10;

    a2 = 11;
    sem_post(&sa2b1);

    sem_wait(&sc1_x);// we are sure that sem_wait(&sc1_x) in this line will only be triggered by sem_post(&sc1_x) in line 
                     // 54 (as intended) because sem_post(&sc1_x) in line 45 requires szy (twice) which is posted two lines below
    y = a1 + c1;
    sem_post(&szy);

    //sem_wait(&sx); 
    sem_wait(&sc1_x); // reusing sc1_x instead because it is not used anymore
                     
    printf("x: %d\n", x);

    return NULL;
}

void* thread2_func(void* arg) {
    b1 = 20;
    sem_post(&sa2b1);

    b2 = 21;

    sem_wait(&sc2);
    w = b2 + c2;

    sem_wait(&szy);
    sem_wait(&szy);
    x = z - y + w;
    //sem_post(&sx);
    sem_post(&sc1_x);
    

    return NULL;
}

void* thread3_func(void* arg) {
    //sleep(3); //even with sleep the order is secured
    c1 = 30;
    sem_post(&sc1_x);

    c2 = 31;
    sem_post(&sc2);

    sem_wait(&sa2b1);
    sem_wait(&sa2b1);
    z = a2 + b1;
    sem_post(&szy);

    return NULL;
}

int main() {
    pthread_t t1, t2, t3;

    // Initialize semaphores
    sem_init(&sa2b1, 0, 0);
    sem_init(&sc1_x, 0, 0);
    sem_init(&sc2, 0, 0);
    sem_init(&szy, 0, 0);
    // Create threads
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);
    pthread_create(&t3, NULL, thread3_func, NULL);

    // Join threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    // Destroy semaphores
    sem_destroy(&sa2b1);
    sem_destroy(&sc1_x);
    sem_destroy(&sc2);
    sem_destroy(&szy);

    return 0;
}