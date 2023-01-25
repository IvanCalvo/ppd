/*
    This program solves Laplace's equation on a regular 2D grid using simple Jacobi iteration.

    The stencil calculation stops when  iter > ITER_MAX
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define ITER_MAX 3000 // number of maximum iterations
#define CONV_THRESHOLD 1.0e-5f // threshold of convergence

// matrix to be solved
double **grid;

// auxiliary matrix
double **new_grid;

// size of each side of the grid
int size;

//chunk for each thread
int chunk;

//number of threads
int num_threads;

int aux_i = 0;
int aux_j = 0;
int t=0;

// return the maximum value
double max(double a, double b){
    if(a > b)
        return a;
    return b;
}

// return the absolute value of a number
double absolute(double num){
    if(num < 0)
        return -1.0 * num;
    return num;
}

// allocate memory for the grid
void allocate_memory(){
    grid = (double **) malloc(size * sizeof(double *));
    new_grid = (double **) malloc(size * sizeof(double *));

    for(int i = 0; i < size; i++){
        grid[i] = (double *) malloc(size * sizeof(double));
        new_grid[i] = (double *) malloc(size * sizeof(double));
    }
}

// initialize the grid
void initialize_grid(){
    // seed for random generator
    srand(10);

    int linf = size / 2;
    int lsup = linf + size / 10;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            // inicializa região de calor no centro do grid
            if ( i>=linf && i < lsup && j>=linf && j<lsup)
                grid[i][j] = 100;
            else
               grid[i][j] = 0;
            new_grid[i][j] = 0.0;
        }
    }
}

// save the grid in a file
void save_grid(){

    char file_name[30];
    sprintf(file_name, "grid_laplace_%dthreads.txt", num_threads);

    // save the result
    FILE *file;
    file = fopen(file_name, "w");

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            fprintf(file, "%lf ", grid[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void *iteration(void *args){
    //calculando a divisao de colunas para cada thread
    //definindo as posicoes de inicio e fim no loop
    int id = *(int *) args;
    int begin = id * chunk;
    int end = begin + chunk;

    //garantindo que a ultima thread realize o restante da divisao
    if(id == num_threads-1) end = size;

    //evitando a pos 0 na matriz, pois causa erro tentando acessar a pos -1
    if (begin == 0) begin = 1;

    //dividindo a matriz em colunas, do tamanho do chunk pre-definido
    for(int i = 1; i < size-1; i++) {
        //escolhendo apenas as colunas pertencentes ao chunk
        for(int j = begin; j < end; j++) {
            new_grid[i][j] = 0.25 * (grid[i][j+1] + grid[i][j-1] +
                                        grid[i-1][j] + grid[i+1][j]);
        }
    }
}

void *update(void *args){
    //calculando a divisao de colunas para cada thread
    //definindo as posicoes de inicio e fim no loop
    int id = *(int *) args;
    int begin = id * chunk;
    int end = begin + chunk;

    //garantindo que a ultima thread realize o restante da divisao
    if(id == num_threads-1) end = size;

    //evitando a pos 0 na matriz, pois causa erro tentando acessar a pos -1
    if (begin == 0) begin = 1;

    for(int i = 1; i < size-1; i++) {
        for(int j = begin; j < end; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }
}

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Usage: ./laplace_seq N T\n");
        printf("N: The size of each side of the domain (grid)\n");
        printf("T: Nnumber of threads\n");
        exit(-1);
    }

    // variables to measure execution time
    struct timeval time_start;
    struct timeval time_end;

    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    pthread_t threads[num_threads];

    int t_id[num_threads];

    // allocate memory to the grid (matrix)
    allocate_memory();

    // set grid initial conditions
    initialize_grid();

    double err = 1.0;
    int iter = 0;

    printf("Jacobi relaxation calculation: %d x %d grid with %d threads\n", size, size, num_threads);

    // get the start time
    gettimeofday(&time_start, NULL);

    //definindo o tamanho do chunk
    chunk = size/num_threads;

    while ( iter <= ITER_MAX ) {
        //criando as threads
        for(int i = 0; i < num_threads; i++){
            t_id[i] = i;
            pthread_create(&threads[i], NULL, iteration, (void *) &t_id[i]);
        }

        //esperando as threads terminarem para a sincronização
        for(int i = 0; i < num_threads; i++){
            pthread_join(threads[i], NULL);
        }

        //criando as threads
        for(int i = 0; i < num_threads; i++){
            t_id[i] = i;
            pthread_create(&threads[i], NULL, update, (void *) &t_id[i]);
        }

        //esperando as threads terminarem para a sincronização
        for(int i = 0; i < num_threads; i++){
            pthread_join(threads[i], NULL);
        }

        iter++;
    }

    // get the end time
    gettimeofday(&time_end, NULL);

    double exec_time = (double) (time_end.tv_sec - time_start.tv_sec) +
                       (double) (time_end.tv_usec - time_start.tv_usec) / 1000000.0;

    //save the final grid in file
    save_grid();

    printf("Kernel executed in %lf seconds with %d iterations and %d threads\n", exec_time, iter, num_threads);

    return 0;
}
