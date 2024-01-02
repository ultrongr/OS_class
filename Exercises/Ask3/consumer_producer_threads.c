#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10
#define P 5
#define C 5

// Declare semaphores
sem_t mutex, full, empty;
int buffer[N]={0};
int buffer_index = 0;

void print_buffer() {
    printf("Buffer: ");
    for(int i=0; i<N; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}

void * producer(void * arg) {
    srand((int)(arg));
    printf("Producer created\n");
    while(1) {
        sem_wait(&empty);
        sem_wait(&mutex);

        // produce an item
        buffer[buffer_index]=rand()%9 +1;
        printf("++Produced: %d\n", buffer[buffer_index]);
        buffer_index++;
        print_buffer();
        
        sem_post(&mutex);
        sem_post(&full);
    }
    printf("Producer exiting\n");
}

void * consumer(void * arg) {
    printf("Consumer created\n");
    while(1) {
        sem_wait(&full);
        sem_wait(&mutex);

        // remove item from buffer
        buffer_index--;
        printf("--Consumed: %d\n", buffer[buffer_index]);
        buffer[buffer_index]=0;
        print_buffer();


        sem_post(&mutex);
        sem_post(&empty);
        

        // consume item
    }
    printf("Consumer exiting\n");
}



int main() {    
    int i;
    pthread_t producers[P];
    pthread_t consumers[C];

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, N);
    print_buffer();
    sleep(1);

    // Create threads
    for(i=0; i<P; i++) {
        pthread_create(&(producers[i]), NULL, producer, (void *)i);
    }
    for(i=0; i<C; i++) {
        pthread_create(&(consumers[i]), NULL, consumer, NULL);
    }


    // Join threads
    for(i=0; i<P; i++) {
        pthread_join(producers[i], NULL);
    }
    for(i=0; i<C; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);

    printf("Main thread exiting\n");
    return 0;
}