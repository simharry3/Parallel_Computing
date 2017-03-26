#!/bin/sh
#SBATCH --partition=medium
#SBATCH --time=00:60:00
#SBATCH --nodes=8
#SBATCH --job-name="A4-Test"

#512 RANKS
srun  --ntasks 512 ./assignment4 1 1 16 25
# srun  --ntasks 512 ./assignment4 1 2 16 25
# srun  --ntasks 512 ./assignment4 1 4 16 25
# srun  --ntasks 512 ./assignment4 1 8 16 25
# srun  --ntasks 512 ./assignment4 1 16 16 25