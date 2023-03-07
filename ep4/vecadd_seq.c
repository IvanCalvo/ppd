/*
    Simple program to add two vectors and verify the results.

    Based on Tim Mattson's (November 2017) implementation.
*/

#include <stdio.h>
#include <stdlib.h>
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

    int size = atoi(argv[1]);
    int err = 0;
    double *a = (double*) malloc(size * sizeof(double));
    double *b = (double*) malloc(size * sizeof(double));
    double *c = (double*) malloc(size * sizeof(double));
    double *res = (double*) malloc(size * sizeof(double));

    // fill the arrays
    for (int i = 0; i < size; i++){
        a[i] = (double) i;
        b[i] = 2.0 * (double) i;
        c[i] = 0.0;
        res[i] = i + 2 * i;
    }

    // add two vectors
    for (int i = 0; i < size; i++){
        c[i] = a[i] + b[i];
    }

    // test results
    for (int i = 0; i < size; i++){
        double val = c[i] - res[i];
        val = val * val;

        if(val > TOL)
            err++;
    }

    // get the end time
    gettimeofday(&time_end, NULL);

    double exec_time = (double) (time_end.tv_sec - time_start.tv_sec) +
                       (double) (time_end.tv_usec - time_start.tv_usec) / 1000000.0;

    printf("vectors added with %d errors in %lf seconds\n", err, exec_time);

    free(a);
    free(b);
    free(c);
    free(res);
    
    return 0;
}
