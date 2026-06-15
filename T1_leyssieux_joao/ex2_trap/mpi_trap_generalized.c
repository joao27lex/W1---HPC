#include <stdio.h>
#include <mpi.h>
#include <math.h>

static double f(double x) { 
    return sin(x); 
}


static double Trap(double left, double right, int n_traps, double h) {
    double estimate = (f(left) + f(right)) / 2.0;
    for (int i = 1; i <= n_traps - 1; i++) {
        double x_i = left + i * h;
        estimate += f(x_i);
    }
    return estimate * h;
}

/* Process 0 reads a, b, n from stdin and forwards them to every other
 * process using a manual loop of MPI_Send. Class 6 will replace this
 * pattern with MPI_Bcast. */
static void Get_input(int my_rank, int comm_sz,
                      double *a_p, double *b_p, int *n_p) {
    if (my_rank == 0) {
        printf("Enter a, b, n: ");
        fflush(stdout);
        scanf("%lf %lf %d", a_p, b_p, n_p);
        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(a_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(b_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(n_p, 1, MPI_INT,    dest, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(a_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(b_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(n_p, 1, MPI_INT,    0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

int main(void) {
    int    my_rank, comm_sz, n, local_n, rem, offset;
    double a, b, h, local_a, local_b, local_integral, total_integral;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    Get_input(my_rank, comm_sz, &a, &b, &n);

    h               = (b - a) / n;
    rem             = n % comm_sz;
    local_n         = n / comm_sz + (my_rank < rem ? 1 : 0);
    offset          = my_rank * (n / comm_sz) + (my_rank < rem ? my_rank : rem);
    local_a         = a + offset * h;
    local_b         = local_a + local_n * h;
    local_integral  = Trap(local_a, local_b, local_n, h);   

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