#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

#define DT 0.0070710676f // delta t
#define DX 15.0f // delta x
#define DY 15.0f // delta y
#define V 1500.0f // wave velocity v = 1500 m/s
#define HALF_LENGTH 1 // radius of the stencil

/*
 * save the matrix on a file.txt
 */
void save_grid(int rows, int cols, int time, float *matrix){

    char file_name[64];
    sprintf(file_name, "wavefield_omp_%d_%d_%d.txt", rows, cols, time);

    // save the result
    FILE *file;
    file = fopen(file_name, "w");

    for(int i = 0; i < rows; i++) {

        int offset = i * cols;

        for(int j = 0; j < cols; j++) {
            fprintf(file, "%f ", matrix[offset + j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);

}


int main(int argc, char* argv[]) {

    if(argc != 4){
        printf("Usage: ./stencil N1 N2 TIME\n");
        printf("N1 N2: grid sizes for the stencil\n");
        printf("TIME: propagation time in ms\n");
        exit(-1);
    }

    // number of rows of the grid
    int rows = atoi(argv[1]);

    // number of columns of the grid
    int cols = atoi(argv[2]);

    // number of timesteps
    int time = atoi(argv[3]);
    
    // calc the number of iterations (timesteps)
    int iterations = (int)((time/1000.0) / DT);

    // represent the matrix of wavefield as an array
    float *prev_base = malloc(rows * cols * sizeof(float));
    float *next_base = malloc(rows * cols * sizeof(float));

    // represent the matrix of velocities as an array
    float *vel_base = malloc(rows * cols * sizeof(float));

    printf("Grid Sizes: %d x %d\n", rows, cols);
    printf("Iterations: %d\n", iterations);

    // ************* BEGIN INITIALIZATION *************

    printf("Initializing ... \n");

    // define source wavelet
    float wavelet[12] = {0.016387336, -0.041464937, -0.067372555, 0.386110067,
                         0.812723635, 0.416998396,  0.076488599,  -0.059434419,
                         0.023680172, 0.005611435,  0.001823209,  -0.000720549};

    // initialize matrix
    for(int i = 0; i < rows; i++){

        int offset = i * cols;

        for(int j = 0; j < cols; j++){
            prev_base[offset + j] = 0.0f;
            next_base[offset + j] = 0.0f;
            vel_base[offset + j] = V * V;
        }
    }

    // add a source to initial wavefield as an initial condition
    for(int s = 11; s >= 0; s--){
        for(int i = rows / 2 - s; i < rows / 2 + s; i++){

            int offset = i * cols;

            for(int j = cols / 2 - s; j < cols / 2 + s; j++)
                prev_base[offset + j] = wavelet[s];
        }
    }

    // ************** END INITIALIZATION **************

    printf("Computing wavefield ... \n");

    float *swap;

    float dxSquared = DX * DX;
    float dySquared = DY * DY;
    float dtSquared = DT * DT;

    // variable to measure execution time
    struct timeval time_start;
    struct timeval time_end;

    // get the start time
    gettimeofday(&time_start, NULL);

    // wavefield modeling
    float* next = malloc(rows * cols * sizeof(float));
    for(int n = 0; n < iterations; n++) {
        #pragma omp parallel for
        for(int i = HALF_LENGTH; i < rows - HALF_LENGTH; i++) {
            int start_index = i * cols + HALF_LENGTH;
            int end_index = start_index + cols - 2 * HALF_LENGTH;
            for(int j = start_index; j < end_index; j++) {
                //neighbors in the horizontal direction
                float value = (prev_base[j + 1] - 2.0 * prev_base[j] + prev_base[j - 1]) / dxSquared;
                
                //neighbors in the vertical direction
                value += (prev_base[j + cols] - 2.0 * prev_base[j] + prev_base[j - cols]) / dySquared;
                
                value *= dtSquared * vel_base[j];
                
                next[j] = 2.0 * prev_base[j] - next_base[j] + value;
            }
        }

        // copy the values back to the main array
        #pragma omp parallel for
        for(int i = HALF_LENGTH; i < rows - HALF_LENGTH; i++) {
            int start_index = i * cols + HALF_LENGTH;
            int end_index = start_index + cols - 2 * HALF_LENGTH;

            for(int j = start_index; j < end_index; j++) {
                prev_base[j] = next[j];
            }
        }

        // swap arrays for next iteration
        swap = next_base;
        next_base = prev_base;
        prev_base = swap;

    }

    free(next);

    // get the end time
    gettimeofday(&time_end, NULL);

    double exec_time = (double) (time_end.tv_sec - time_start.tv_sec) + (double) (time_end.tv_usec - time_start.tv_usec) / 1000000.0;

    save_grid(rows, cols, time, next_base);

    printf("Iterations completed in %f seconds \n", exec_time);

    free(prev_base);
    free(next_base);
    free(vel_base);

    return 0;
}
