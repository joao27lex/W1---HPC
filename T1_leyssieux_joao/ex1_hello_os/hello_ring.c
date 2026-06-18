#define _GNU_SOURCE

#include <unistd.h>   // getpid()
#include <sched.h>    // sched_getcpu()
#include <mpi.h>

//default libs
#include <stdio.h>
#include <string.h>


#define MAX_STRING 100

//returns the destination rank for process k
int dest(int k, int comm_size) {
    return (k + 1) % comm_size;
}

//returns the source rank for process k
int source(int k, int comm_size){
    return (k - 1 + comm_size) % comm_size;
}

int main(void){
    MPI_Init(NULL, NULL);

    int comm_size; //total number of processes in the communicator
    int my_rank; //this process’s rank 
    pid_t my_pid; //proccess identifier
    int my_cpu; //get cpu which the prcess is running on
    char greeting[MAX_STRING]; //size of the greeting_recv string
    int destination_value; 
    int source_value; 

    MPI_Comm_size(MPI_COMM_WORLD, &comm_size); // how many processes?
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // which process am i?

    my_pid = getpid();
    my_cpu = sched_getcpu();

    destination_value = dest(my_rank, comm_size); // calculates the destination rank for the current process
    source_value = source(my_rank, comm_size); // calculates the source rank for the current process

    snprintf(greeting, MAX_STRING, "Hello from rank %d/%d -- PID = %d, CPU = %d", my_rank, comm_size, my_pid, my_cpu); 

    if(my_rank == 0){
        printf("%s\n", greeting);
    }

    for(int i = 1; i< comm_size; i++){
        // the function MPI_Sendrecv_replace receives only one buffer as parameter
        MPI_Sendrecv_replace(greeting, MAX_STRING, MPI_CHAR, destination_value, 0, source_value, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // sends the greeting to the destination and receives the greeting from the source. replaces the content of 'greeting' with the received message
    }
    
    if(my_rank == 0){
        printf("%s\n", greeting);  
    }

    MPI_Finalize();
    return 0;
}
