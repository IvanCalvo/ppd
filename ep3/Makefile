CC=gcc

all: clean openmp

openmp:
	$(CC) laplace_openmp.c -o laplace_openmp -fopenmp

clean:
	rm -f laplace_openmp