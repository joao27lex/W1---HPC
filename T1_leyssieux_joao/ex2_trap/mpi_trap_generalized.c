#include <stdio.h>
#include <mpi.h>
#include <math.h>

// this is the function that will be integrated. in this case, it is sin(x)
double f(double x) { 
    return sin(x); 
}

// applies the trapezoid formula to a section given interval
double Trap(double left, double right, int n_traps, double h) {
    double estimate = (f(left) + f(right)) / 2.0; // estimate starts with the average of the left and right points (y-axis)
    for (int i = 1; i <= n_traps - 1; i++) {
        double x_i = left + i * h; //calculates the x value of the i-th trapezoid (x-axis)
        estimate += f(x_i);
    }
    return estimate * h; //returns estimate (y-axis) multiplied by the width of the trapezoids (x-axis)
}

// Process 0 reads a, b, n from stdin and sends to the other processes
// they return the local double to process 0 at the end

void Get_input(int my_rank, int comm_sz, double *a_p, double *b_p, int *n_p) {
    if (my_rank == 0) {
        printf("Enter a, b, n: \n");
        scanf("%lf %lf %d", a_p, b_p, n_p); //inputs left and right points and total number of trapezoids
        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(a_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD); //sends left point to all other processes
            MPI_Send(b_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD); // sends right point to all other processes
            MPI_Send(n_p, 1, MPI_INT,    dest, 0, MPI_COMM_WORLD); // sends total number of trapezoids to all other processes
        }
    }
    else {
        MPI_Recv(a_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receives left point from process 0
        MPI_Recv(b_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receives right point from process 0
        MPI_Recv(n_p, 1, MPI_INT,    0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receives total number of trapezoids from process 0
    }
}

int main(void) {
    int my_rank, comm_sz, n, local_n, rem, offset;
    double a, b, h, local_a, local_b, local_integral, total_integral, T_start, T_end, T_p, E_p;

    MPI_Init(NULL, NULL);
    T_start = MPI_Wtime(); // starts the timer after the initialization of the MPI

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); //how many processes?
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); //which one am i?

    Get_input(my_rank, comm_sz, &a, &b, &n);

    h               = (b - a) / n; // calculates the width of each trapezoid
    rem             = n % comm_sz; // calculates the remainder to determine how the traps will be distributed
    local_n         = n / comm_sz + (my_rank < rem ? 1 : 0); //calculates how many traps each process will calculate
    offset          = my_rank * (n / comm_sz) + (my_rank < rem ? my_rank : rem); //calculates where each process will start calculating the traps
    local_a         = a + offset * h; //calculates the left point of the interval that each process will calculate
    local_b         = local_a + local_n * h; //calculates the right point of the interval that each process will calculate
    local_integral  = Trap(local_a, local_b, local_n, h); //calculates the integral of the local interval usign trap function

    if (my_rank != 0){
        MPI_Send(&local_integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } 

    //process 0 receives the local integrals from the other processes 
    else{
        total_integral = local_integral;
        for (int src = 1; src < comm_sz; src++){
            MPI_Recv(&local_integral, 1, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_integral += local_integral;
        }
        printf("\nWith n = %d trapezoids, integral of f on [%g, %g] = %.15e\n", n, a, b, total_integral);
    }

    T_end = MPI_Wtime(); //stops the timer
    MPI_Finalize();

    T_p = T_end - T_start; // Calculates the execution time
    //E_p = T_s/T_p;

    if (my_rank == 0) {
        printf("\nExecution time -  %f seconds\n", T_p);
    }

    return 0;
}