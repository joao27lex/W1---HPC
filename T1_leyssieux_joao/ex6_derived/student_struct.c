#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> //offsetof function
#include <mpi.h>

struct Student {
    char name[50];
    double grade;
    int id;
};

int main(void) {
    int my_rank, comm_sz;
    struct Student student; 

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    MPI_Datatype mpi_student_type;                              // stores the new MPI datatype for the struct
    int block_lengths[3] = {50, 1, 1};                          // the number of each element in the struct
    MPI_Datatype types[3] = {MPI_CHAR, MPI_DOUBLE, MPI_INT};    // the corresponding tyhpes of the struct fields
    MPI_Aint displacements[3];                                  // the byte displacement of each parameter in the struct

    displacements[0] = offsetof(struct Student, name); // 0
    displacements[1] = offsetof(struct Student, grade); // 50
    displacements[2] = offsetof(struct Student, id); // 58

    // creates the new MPI type for the struct -> associates block_length, displacement and type for each parameter of the struct
    MPI_Type_create_struct(3, block_lengths, displacements, types, &mpi_student_type);
    
    // 'saves' the new MPI type
    MPI_Type_commit(&mpi_student_type);
    
    if (my_rank == 0) {
        scanf("%50s %lf %d", student.name, &student.grade, &student.id); // reads the student data from the user input
        printf("Name: %s grade: %.1f  id: %d\n", student.name, student.grade, student.id);
    }

    // every process calls the broadcast, only the root sends the data and the others receive the data
    MPI_Bcast(&student, 1, mpi_student_type, 0, MPI_COMM_WORLD);

    //prints the data received by the other processes exccept the root
    if (my_rank != 0) {
        printf("Process %d name: %s grade: %.1f id: %d\n", my_rank, student.name, student.grade, student.id);
    }

    MPI_Type_free(&mpi_student_type); // frees the memory alocatted for the MPI Type
    MPI_Finalize();

    return 0;
}