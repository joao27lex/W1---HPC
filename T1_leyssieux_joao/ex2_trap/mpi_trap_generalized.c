#include <stdio.h>
#include <mpi.h>
#include <math.h>

// this is the function that will be integrated. in this case, it is sin(x)
static double f(double x) { 
    return sin(x); 
}

// applies the trapezoid formula to a section given interval
static double Trap(double left, double right, int n_traps, double h) {
    double estimate = (f(left) + f(right)) / 2.0; // estimate starts with the average of the left and right points (y-axis)
    for (int i = 1; i <= n_traps - 1; i++) {
        double x_i = left + i * h; //calculates the x value of the i-th trapezoid (x-axis)
        estimate += f(x_i);
    }
    return estimate * h; //returns estimate (y-axis) multiplied by the width of the trapezoids (x-axis)
}

/* Process 0 reads a, b, n from stdin and forwards them to every other
 * process using a manual loop of MPI_Send. Class 6 will replace this
 * pattern with MPI_Bcast. */
static void Get_input(int my_rank, int comm_sz,
                      double *a_p, double *b_p, int *n_p) {
    if (my_rank == 0) {
        printf("Enter a, b, n: ");
        fflush(stdout);
        scanf("%lf %lf %d", a_p, b_p, n_p); //inputs left and right points and total number of trapezoids
        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(a_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD); //sends left point to all other processes
            MPI_Send(b_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD); // sends right point to all other processes
            MPI_Send(n_p, 1, MPI_INT,    dest, 0, MPI_COMM_WORLD); // sends total number of trapezoids to all other processes
        }
    } else {
        MPI_Recv(a_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receives left point from process 0
        MPI_Recv(b_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receives right point from process 0
        MPI_Recv(n_p, 1, MPI_INT,    0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receives total number of trapezoids from process 0
    }
}

int main(void) {
    int    my_rank, comm_sz, n, local_n, rem, offset;
    double a, b, h, local_a, local_b, local_integral, total_integral;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    Get_input(my_rank, comm_sz, &a, &b, &n);

    h               = (b - a) / n; // calculates the width of each trapezoid
    rem             = n % comm_sz; // calculates the mod of the total number of traps by the number of processes to determine how many processes will it need to calculate an extra trap
    local_n         = n / comm_sz + (my_rank < rem ? 1 : 0); //calculates how many traps each process will calculate
    offset          = my_rank * (n / comm_sz) + (my_rank < rem ? my_rank : rem); //calculates where each process will start calculating the traps
    local_a         = a + offset * h; //calculates the left point of the interval that each process will calculate
    local_b         = local_a + local_n * h; //calculates the right point of the interval that each process will calculate
    local_integral  = Trap(local_a, local_b, local_n, h); //calculates the integral of the local interval usign trap function

    if (my_rank != 0) {
        MPI_Send(&local_integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        total_integral = local_integral;
        for (int src = 1; src < comm_sz; src++) {
            MPI_Recv(&local_integral, 1, MPI_DOUBLE, src, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_integral += local_integral;
        }
        printf("With n = %d trapezoids, integral of f on [%g, %g] = %.15e\n",
               n, a, b, total_integral);
    }

    MPI_Finalize();
    return 0;
}