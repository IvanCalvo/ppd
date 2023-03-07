/*
    Simple program to add two vectors and verify the results.

    Based on Tim Mattson's (November 2017) implementation.
*/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <sys/time.h>
#define TOL 0.0000001

int main(int argc, char *argv[]) {

    if(argc != 2){
        printf("Usage: ./vecadd_seq N\n");
        printf("N: Size of the vectors\n");
        exit(-1);
    }

    // variables to measure execution time
    struct timeval time_start;
    struct timeval time_end;

    // get the start time
    gettimeofday(&time_start, NULL);

    double *a;
    double *b;
    double *c;
    double *res;

    int size = atoi(argv[1]);
    int err = 0;

    int myid, nprocs;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    double *ap;
    double *bp;
    double *cp;

    if (myid == 0){
        a = (double*) malloc(size * sizeof(double));
        b = (double*) malloc(size * sizeof(double));
        c = (double*) malloc(size * sizeof(double));
        res = (double*) malloc(size * sizeof(double));

        // fill the arrays
        for (int i = 0; i < size; i++){
            a[i] = (double) i;
            b[i] = 2.0 * (double) i;
            c[i] = 0.0;
            res[i] = i + 2 * i;
        }
    }

    int num_local_size = size/nprocs;

    ap = (double*) malloc(num_local_size * sizeof(double));
    bp = (double*) malloc(num_local_size * sizeof(double));
    cp = (double*) malloc(num_local_size * sizeof(double));

    MPI_Scatter(a, num_local_size, MPI_DOUBLE, ap, num_local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, num_local_size, MPI_DOUBLE, bp, num_local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for(int i=0;i<num_local_size;i++)
		cp[i] = ap[i]+bp[i];

    MPI_Gather(cp, num_local_size, MPI_DOUBLE, c, num_local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (myid == 0)  {
        for (int i = 0; i < size; i++){
            double val = c[i] - res[i];
            val = val * val;

            if(val > TOL)
                err++;
        }

        gettimeofday(&time_end, NULL);

        double exec_time = (double) (time_end.tv_sec - time_start.tv_sec) +
                        (double) (time_end.tv_usec - time_start.tv_usec) / 1000000.0;

        printf("vectors added with %d errors in %lf seconds using %d procs\n", err, exec_time, nprocs);
	}

    if(myid == 0) {
        free(a);
        free(b);
        free(c);
        free(res);

    }

    free(ap);
    free(bp);
    free(cp);

    MPI_Finalize();
    return 0;
}
