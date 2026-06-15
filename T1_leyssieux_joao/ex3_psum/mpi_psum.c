#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <mpi.h>
#include <math.h>

double genRandomDouble(void){
    return (double)rand() / RAND_MAX;
}

int main(int argc, char** argv){
    int my_rank, comm_sz, element_per_process, offset;
    double local_sum, total_sum, received_sum, serial_sum;
    
    double *randomVector = NULL;
    double *localVector = NULL;
    
    int n = 1000; // Fixed size to simplify memory allocation and division

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

    element_per_process = n / comm_sz;

    localVector = (double*)malloc(element_per_process * sizeof(double));

    // BCast step ------------------------------------------------------------------------------------

    if (my_rank == 0) {
        // Allocates and generates random double values to the vector
        randomVector = (double*)malloc(n * sizeof(double));
        srand(time(NULL));

        for (int i = 0; i < n; i++){
            randomVector[i] = genRandomDouble(); 
        }

        // Distributes splitted vector to all processes
        for (int process = 1; process < comm_sz; process++){
            offset = process * element_per_process;
            MPI_Send(&randomVector[offset], element_per_process, MPI_DOUBLE, process, 0, MPI_COMM_WORLD);
        }

        // Copies the splitted vector to the local vector of process
        for (int i = 0; i < element_per_process; i++){
            localVector[i] = randomVector[i];
        }
    } 
    
    else {
        // Receives the splitted vector     
        MPI_Recv(localVector, element_per_process, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    //----------------------------------------------------------------------------------------------------------------

    local_sum = 0.0;
    for (int i = 0; i < element_per_process; i++){
        local_sum += localVector[i];
    }

    //----------------------------------------------------------------------------------------------------------------

    // Reduce Step ------------------------------------------------------------------------------------
    if (my_rank != 0) {
        // Sends partial sum to process 0
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } 
    
    else {
        total_sum = local_sum;

        // Receives partial sums and calculates total
        for (int process = 1; process < comm_sz; process++){
            MPI_Recv(&received_sum, 1, MPI_DOUBLE, process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_sum += received_sum;
        }

        // Calculates serial sum for comparison
        serial_sum = 0.0;
        for(int i = 0; i < n; i++){
            serial_sum += randomVector[i];
        }

        printf("Parallel sum: %f\n", total_sum);
        printf("Serial sum: %f\n", serial_sum);
        printf("Relative error: %e\n", fabs(serial_sum - total_sum));

        free(randomVector);
    }

    free(localVector);
    MPI_Finalize();
    
    return 0;
}