#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>




int main(int argc, char *argv[]) {
    int fd[2];

    // create pipe
    pipe(fd);

    if (argc != 3) {
        printf("Usage: %s <N> <n>\n", argv[0]);
        return 1; // Exit with an error code
    }

    // Convert command line arguments to integers
    int N = atoi(argv[1]);
    int n = atoi(argv[2]);



    printf("Sum of the first %d integers, calculated by %d processes:\n", N, n);


    for (int i=0; i < n; i++) {
        
        int pid = fork();
        
        if (pid == 0) { // child process
            
            // calculate the sum of the i-th part
            int sum = 0;
            if(i==n-1){ // last child process has to calculate the rest of the sum
                for (int k = i*(N/n); k <=N; k++){
                    sum +=k;
                }
            }
            else{ // other child processes calculate the sum of the i-th part
                for (int k = i*(N/n); k <(i+1)*(N/n); k++){
                    sum +=k;
                }
            }
            // i-th child process writes the sum in the pipe and exits
            close(fd[0]); // write, close read
            write(fd[1], &sum, sizeof(sum));
            close(fd[1]); // close write
            exit(0); // exit child process
        }
    }

    // parent process calculates the actual sum
    int parent_sum = N*(N+1)/2;
    
    
    close(fd[1]); // read, close write
    
    int children_sum = 0; // the sum of the sums of all the child processes
    int val;
    for (int i=0; i < n; i++) {
        read(fd[0], &val, sizeof(val)); // waiting
        printf("Parent receives value %d from child %d\n", val, i);
        children_sum += val;
    }

    printf("The total child_sum is %d\n", children_sum);
    printf("Computed parent_sum by parent process is %d\n", parent_sum);
    close(fd[0]); // close read
    return 0;
}

