#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <mpi.h>
#include <math.h>

// Generates a random double number between 0 and 1
double genRandomDouble(void){
    return (double)rand() / RAND_MAX;
}

int main(void){
    int my_rank, comm_sz, element_per_process, offset;
    double local_sum, total_sum, received_sum, serial_sum;
    
    double *randomVector = NULL;
    double *localVector = NULL;
    
    int n = 1000;

    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

    element_per_process = n / comm_sz; // N/p

    localVector = (double*)malloc(element_per_process * sizeof(double)); // allocates local vector for each process

    // BCast step ------------------------------------------------------------------------------------

    if (my_rank == 0){
        randomVector = (double*)malloc(n * sizeof(double)); // allocates and generates random double values to the vector
        srand(time(NULL)); //seed for the random generator

        for (int i = 0; i < n; i++){
            randomVector[i] = genRandomDouble(); //stores a random double in the i position of the cector
        }

        if (n%comm_sz == 0){
            for (int process = 1; process < comm_sz; process++){
                offset = process * element_per_process; 

                // sends the corresponding part of the vector to each process
                MPI_Send(&randomVector[offset], element_per_process, MPI_DOUBLE, process, 0, MPI_COMM_WORLD);
            }

            // Copies the splitted vector to the local vector of process
            for (int i = 0; i < element_per_process; i++){
                localVector[i] = randomVector[i];
            }
        }
        else{
            int rem = n % comm_sz; // calculates the mod to increment the offset

            for (int process = 1; process < comm_sz; process++){
                offset = process * element_per_process + (process < rem ? process : rem); // calculates the offset for the process considering the extra elements
                int elements_to_send = element_per_process + (process < rem ? 1 : 0); // calculates how many elements the process will receive considering the extra elements

                //
                MPI_Send(&randomVector[offset], elements_to_send, MPI_DOUBLE, process, 0, MPI_COMM_WORLD);
            }

            // Copies the splitted vector to the local vector of process
            for (int i = 0; i < element_per_process + (my_rank < rem ? 1 : 0); i++){
                localVector[i] = randomVector[i];
            }
        }
        
    } 
    
    // if not process 0, receives the splitted vector
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