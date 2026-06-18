//  While the other program uses the MPI_Datatype to broadcast the struct in one call, 
//  this program sends each struct parameter individually using three separate MPI_Bcast

#include <stdio.h>
#include <stdlib.h>
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

    if (my_rank == 0){
        scanf("%49s %lf %d", student.name, &student.grade, &student.id); //reads the student data from the user input
        printf("Name: %s grade: %.1f  id: %d\n", student.name, student.grade, student.id); 
    }

    MPI_Bcast(student.name, 50, MPI_CHAR, 0, MPI_COMM_WORLD);  //bcasts the char array 

    MPI_Bcast(&student.grade, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); //bcasts the double value

    MPI_Bcast(&student.id, 1, MPI_INT, 0, MPI_COMM_WORLD); //bcasts the int value

    if (my_rank != 0) {
        printf("Process %d -> name: %s grade: %.1f id: %d\n", my_rank, student.name, student.grade, student.id); 
    }

    MPI_Finalize();

    return 0;
}