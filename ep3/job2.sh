#!/bin/bash
#SBATCH -J laplaceOpenMP            # Job name
#SBATCH -p fast                     # Job partition
#SBATCH -n 1                        # Number of processes
#SBATCH -t 01:30:00                 # Run time (hh:mm:ss)
#SBATCH --cpus-per-task=40          # Number of CPUs per process
#SBATCH --output=%x.%j.out          # Name of stdout output file - %j expands to jobId and %x to jobName
#SBATCH --error=%x.%j.err           # Name of stderr output file

echo "*** OPENMP ***"
for i in 1 2 4 8 16 24 32 36 40
do
    export OMP_NUM_THREADS=$i
    srun singularity run container.sif laplace_openmp 2000
done