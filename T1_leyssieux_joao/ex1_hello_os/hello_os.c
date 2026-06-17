#define _GNU_SOURCE

#include <unistd.h>   // getpid()
#include <sched.h>    // sched_getcpu()
#include <mpi.h>

//default libs
#include <stdio.h>
#include <string.h>


#define MAX_STRING 100

int main(void){
    MPI_Init(NULL, NULL);

    int comm_size; //total number of processes in the communicator
    int my_rank; //this process’s rank 
    pid_t my_pid; //proccess identifier
    int my_cpu;
    char greeting[MAX_STRING]; //size of the greeting string

    MPI_Comm_size(MPI_COMM_WORLD, &comm_size); //how many processes?
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); //which one am I?

    my_pid = getpid();
    my_cpu = sched_getcpu();

    if(my_rank == 0){
        printf("Hello from rank %d/%d -- PID = %d, CPU = %d\n", my_rank, comm_size, my_pid, my_cpu);
        for(int k = 1; k < comm_size; k++){
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receives greeting from the other processes
            printf("%s\n", greeting);
        };

    }

    else{
        snprintf(greeting, MAX_STRING, "Hello from rank %d/%d -- PID = %d, CPU = %d", my_rank, comm_size, my_pid, my_cpu); //stores the message in 'greeting' and prevents buffer overflow
        MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD); //sends the greeting to p0
    }

    MPI_Finalize();
    return 0;
}
