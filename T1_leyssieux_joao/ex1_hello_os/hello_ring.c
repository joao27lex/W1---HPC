#define _GNU_SOURCE

#include <unistd.h>   // getpid()
#include <sched.h>    // sched_getcpu()
#include <mpi.h>

//default libs
#include <stdio.h>
#include <string.h>


#define MAX_STRING 100

int main(int argc, char* argv[]){
    MPI_Init(&argc, &argv);

    int comm_size; //total number of processes in the communicator
    int my_rank; //this process’s rank 
    pid_t my_pid; //proccess identifier
    int my_cpu;
    char greeting_send[MAX_STRING]; //size of the greeting_send string
    char greeting_recv[MAX_STRING]; //size of the greeting_recv string
    int destination;
    int source; 

    MPI_Comm_size(MPI_COMM_WORLD, &comm_size); // how many processes?
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // which one am I?

    my_pid = getpid();
    my_cpu = sched_getcpu();

    if(my_rank == 0){
        snprintf(greeting_send, MAX_STRING, "Hello from rank %d/%d -- PID = %d, CPU = %d\n", my_rank, comm_size, my_pid, my_cpu);
        for(int k = 1; k < comm_size; k++){
            destination = dest(k, comm_size);
            source = source(k, comm_size);

            MPI_Sendrecv(greeting_send, MAX_STRING, MPI_CHAR, destination, 0, greeting_recv, MAX_STRING, MPI_CHAR, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receives and sends greeting to the other processes
            printf("%s\n", greeting_recv);
        };

    }

    else{
        snprintf(greeting_send, MAX_STRING, "Hello from rank %d/%d -- PID = %d, CPU = %d", my_rank, comm_size, my_pid, my_cpu); //stores the message in 'greeting' and prevents buffer overflow

        destination = dest(my_rank, comm_size);
        source = source(my_rank, comm_size);

        MPI_Sendrecv(greeting_send, MAX_STRING, MPI_CHAR, destination, 0, greeting_recv, MAX_STRING, MPI_CHAR, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receives and sends greeting to the other processes
        printf("%s\n", greeting_recv);
    }

    MPI_Finalize();
    return 0;
}

int dest(int k, int comm_size) //returns the destination rank for process k
{
    return (k + 1) % comm_size;
}

int source(int k, int comm_size) //returns the source rank for process k
{
    return (k - 1 + comm_size) % comm_size;
}