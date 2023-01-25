#!/bin/bash
#SBATCH -J piFactorial              # Job name
#SBATCH -p fast                     # Job partition
#SBATCH -n 1                        # Number of processes
#SBATCH -t 01:30:00                 # Run time (hh:mm:ss)
#SBATCH --cpus-per-task=40          # Number of CPUs per process
#SBATCH --output=%x.%j.out          # Name of stdout output file - %j expands to jobId and %x to jobName
#SBATCH --error=%x.%j.err           # Name of stderr output file

echo "*** SEQUENTIAL ***"
srun singularity run container.sif pi_seq 1000000000

echo "*** PTHREAD ***"
echo "1 thread"
srun singularity run container.sif pi_pth 1000000000 1
echo "2 threads"
srun singularity run container.sif pi_pth 1000000000 2
echo "5 threads"
srun singularity run container.sif pi_pth 1000000000 5
echo "10 threads"
srun singularity run container.sif pi_pth 1000000000 10
echo "20 threads"
srun singularity run container.sif pi_pth 1000000000 20
echo "40 threads"
srun singularity run container.sif pi_pth 1000000000 40

echo "*** OPENMP ***"
echo "1 thread"
export OMP_NUM_THREADS=1
srun singularity run container.sif pi_omp 1000000000
echo "2 threads"
export OMP_NUM_THREADS=2
srun singularity run container.sif pi_omp 1000000000
echo "5 threads"
export OMP_NUM_THREADS=5
srun singularity run container.sif pi_omp 1000000000
echo "10 threads"
export OMP_NUM_THREADS=10
srun singularity run container.sif pi_omp 1000000000
echo "20 threads"
export OMP_NUM_THREADS=20
srun singularity run container.sif pi_omp 1000000000
echo "40 threads"
export OMP_NUM_THREADS=40
srun singularity run container.sif pi_omp 1000000000