#!/bin/bash
#SBATCH -J laplaceEquation          # Job name
#SBATCH -p fast                     # Job partition
#SBATCH -n 1                        # Number of processes
#SBATCH -t 01:30:00                 # Run time (hh:mm:ss)
#SBATCH --cpus-per-task=40          # Number of CPUs per process
#SBATCH --output=%x.%j.out          # Name of stdout output file - %j expands to jobId and %x to jobName
#SBATCH --error=%x.%j.err           # Name of stderr output file

echo " "
srun singularity run container.sif laplace_pth 2000 1
echo " "
srun singularity run container.sif laplace_pth 2000 2
echo " "
srun singularity run container.sif laplace_pth 2000 5
echo " "
srun singularity run container.sif laplace_pth 2000 10
echo " "
srun singularity run container.sif laplace_pth 2000 20
echo " "
srun singularity run container.sif laplace_pth 2000 40