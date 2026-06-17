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

    int comm_size, my_rank, my_cpu;
    pid_t my_pid; //proccess identifier
    char greeting[MAX_STRING]; //size of the greeting string

    MPI_Comm_size(MPI_COMM_WORLD, &comm_size); //how many processes?
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); //which one am I?

    my_pid = getpid(); //gets the process identifier
    my_cpu = sched_getcpu();

    const char *message = "Hello from rank %d/%d -- PID = %d, CPU = %d"; //stores the pointer to the message format

    snprintf(greeting, MAX_STRING, message, my_rank, comm_size, my_pid, my_cpu); //stores the message in 'greeting' 

    char *gather_greetings = NULL; //pointer to store the greetings gathered by process 0
    if(my_rank == 0){
        gather_greetings = (char*)malloc(comm_size * MAX_STRING * sizeof(char)); //allocates memory to store the greetings of all processes in process 0 
    }

    MPI_Gather(greeting, MAX_STRING, MPI_CHAR, gather_greetings, MAX_STRING, MPI_CHAR, 0, MPI_COMM_WORLD);  

    if(my_rank == 0){
        for(int i = 0; i < comm_size; i++){
            printf("%s\n", &gather_greetings[i * MAX_STRING]); //prints the greeting of each process by acessing the string
        }
    }

    free(gather_greetings); //clears the memory allocated for the gathered greetings

    MPI_Finalize();
    return 0;
}
