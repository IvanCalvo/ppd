#!/bin/bash
#SBATCH -J waves_calc                     # Job name
#SBATCH -p fast                     # Job partition
#SBATCH -n 1                        # Number of processes
#SBATCH -t 03:30:00                 # Run time (hh:mm:ss)
#SBATCH --cpus-per-task=40          # Number of CPUs per process
#SBATCH --output=%x.%j.out          # Name of stdout output file - %j expands to jobId and %x to jobName
#SBATCH --error=%x.%j.err           # Name of stderr output file

echo "*** SEQUENTIAL ***"
srun singularity run container.sif wave_seq 5000 5000 10000

echo "*** OPENMP ***"
for i in 1 2 5 10 20 40
do
    echo "*** THREADS = $i***"
    export OMP_NUM_THREADS=$i
    srun singularity run container.sif wave_openMP 5000 5000 10000
done

echo "***Configuracao CPU***"
    lscpu





