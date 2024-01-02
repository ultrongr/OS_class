include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define N 10
#define P 2
#define C 2



const char *mutex = "mutex";
const char *full = "full";
const char *empty = "empty";

sem_t *mutex_id;
sem_t *full_id;
sem_t *empty_id;


void print_buffer(int * buffer) {
    printf("Buffer: ");
    for(int i=0; i<N; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}

void producer(int * buffer, int * buffer_index_ptr){
    int i;
    sem_t *mutex_id = sem_open(mutex, O_CREAT, 0600, 1);
    sem_t *full_id = sem_open(full, O_CREAT, 0600, 0);
    sem_t *empty_id = sem_open(empty, O_CREAT, 0600, N);
//    printf("producer\n");
    sleep(1);
    for(i=0; i<10000; i++){
        sem_wait(empty_id);
        sem_wait(mutex_id);
        buffer[*buffer_index_ptr]=rand()%9+1;
        printf("++Produced: %d\n", *buffer_index_ptr);
        (*buffer_index_ptr)++;
        print_buffer(buffer);


        sem_post(mutex_id);
        sem_post(full_id);


    }
}


void consumer(int * buffer, int * buffer_index_ptr)
{
    int i;
    sem_t *mutex_id = sem_open(mutex, O_CREAT, 0600, 1);
    sem_t *full_id = sem_open(full, O_CREAT, 0600, 0);
    sem_t *empty_id = sem_open(empty, O_CREAT, 0600, N);

    for(i=0; i<10000; i++){

        sem_wait(full_id);
        sem_wait(mutex_id);

        (*buffer_index_ptr)--;
        printf("--Consumed: %d\n", buffer[*buffer_index_ptr]);
        buffer[*buffer_index_ptr]=0;
        print_buffer(buffer);

        sem_post(mutex_id);
        sem_post(empty_id);

    }
    return;

}

int main(int argc, char *argv[])
{
    pid_t pid;
    int i;
    int buffer_id;
    int buffer_index_id;


    buffer_id = shmget(IPC_PRIVATE, N*sizeof(int), SHM_R | SHM_W);
    buffer_index_id = shmget(IPC_PRIVATE, sizeof(int *), SHM_R | SHM_W);
    for(i=0; i<P; i++){

        pid = fork();
        if (pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (!pid){
            producer((int *)shmat(buffer_id, NULL, 0), (int *)shmat(buffer_index_id, NULL, 0));
            return 0;
        }
    }

    for(i=0; i<C; i++){
        pid = fork();
        if (pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (!pid){
            //consumer((int *)shmat(buffer_id, NULL, 0));
            consumer((int *)shmat(buffer_id, NULL, 0), (int *)shmat(buffer_index_id, NULL, 0));
            return 0;
        }
    }
    
    sleep(5);
    printf("unlinking\n");
    sem_unlink(mutex);
    sem_unlink(full);
    sem_unlink(empty);

    return 0;
}