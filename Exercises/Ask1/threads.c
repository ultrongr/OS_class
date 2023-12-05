#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>





// global variables: accessible to all threads
int thread_count;
int N;
int *arr;


// thread function
void *work(void* rank) {
    long tid = (long) rank;

    // calculate the sum of the i-th part
    int thread_sum = 0;
    if(tid==thread_count-1){ // last child thread has to calculate the rest of the sum
        for (int k = tid*(N/thread_count); k <=N; k++){
            thread_sum +=k;
        }
    }
    else{ // other child thread calculate the sum of the i-th part
        for (int k = tid*(N/thread_count); k <(tid+1)*(N/thread_count); k++){
            thread_sum +=k;
        }
    
    }
    arr[tid]=thread_sum;

    return NULL;
}


int main(int argc, char* argv[]) {
    long t;
    pthread_t* thread_handles;
    
    if (argc != 3) {
        printf("Usage: %s <N> <n>\n", argv[0]);
        return 1; // Exit with an error code
    }

    // Convert command line arguments to integers
    N = atoi(argv[1]);
    thread_count= atoi(argv[2]);

    printf("Sum of the first %d integers, calculated by %d threads:\n", N, thread_count);


    thread_handles = malloc (thread_count * sizeof(pthread_t));
    
    // global array
    arr = malloc (thread_count * sizeof(int));
    
    // create thread_count independent threads
    for (t = 0; t < thread_count; t++){
        pthread_create(&thread_handles[t], NULL, work, (void*) t);
    }

    printf("---Hello from the main thread---\n");
    
    // wait until threads are completed
    for (t = 0; t < thread_count; t++) {
        pthread_join(thread_handles[t], NULL);
    }

    // calculate the actual sum of the first N integers
    int process_sum = N*(N+1)/2;


    // sum of the sums of all the threads
    int sum=0;
    for (t = 0; t < thread_count; t++) {
        sum+=arr[t];
        printf("\tthread %ld returns %d\n" ,t ,arr[t]);
    }
    printf("Sum of the first %d integers (using threads) = %d\n", N, sum);
    printf("The process calculated %d\n", process_sum);
    free(thread_handles);
    free(arr);
    return 0;
}