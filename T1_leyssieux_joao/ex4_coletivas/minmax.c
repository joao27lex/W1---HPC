#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <mpi.h>
#include <math.h>

int main(void){
    int my_rank, comm_sz, splitted_block_size, rem_block_size;
    double local_max, local_min, global_max, global_min;
    double *randomRootVector = NULL; 
    double *splittedVector = NULL;
    double serial_min = 0.0;
    double serial_max = 0.0;

    int n = 1000;
    
    MPI_Init(NULL, NULL);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    block_size = n / comm_sz;
    rem_block_size = n % comm_sz;
    
    randomRootVector = (double*)malloc(n * sizeof(double)); //allocates memory for the random doubles in process 0
    splittedVector = (double*)malloc(block_size * sizeof(double)); //allocates memory for the splitted vectors in each process

    if(my_rank == 0){
        srand(time(NULL));
        for (int i = 0; i < n; i++){
            randomRootVector[i] = (double)rand() / RAND_MAX; //generates a random double and stores it in the i position of the vector
            if(randomRootVector[i] < serial_min){
                serial_min = randomRootVector[i]; //updates the serial_min if it's smaller than the current value
            }
            if(randomRootVector[i] > serial_max){
                serial_max = randomRootVector[i]; //updates the serial_max if it's bigger than the current value
            }
        }
    }

    MPI_Scatter(randomRootVector, block_size, MPI_DOUBLE, splittedVector, block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);


//------------------------------------------------------------------------------------------------------------------------------------

    local_min = splittedVector[0]; //starts with the first value
    local_max = splittedVector[0]; //starts with the first value

    for(int i = 1; i < block_size; i++){
        if(splittedVector[i] < local_min){
            local_min = splittedVector[i];
        }
        if(splittedVector[i] > local_max){
            local_max = splittedVector[i];
        }
    }

    MPI_Reduce(&local_min, &global_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);  //global_min -> local_min
    MPI_Reduce(&local_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); //global_max -> local_max

//--------------------------------------------------------------------------------------------------------------------------------------------

    if(my_rank == 0){
        printf("Serial min: %f\n", serial_min);
        printf("Serial max: %f\n", serial_max);
        printf("Global min: %f\n", global_min);
        printf("Global max: %f\n", global_max);
    }

    free(randomRootVector);
    free(splittedVector);

    MPI_Finalize();
    return 0;   
}