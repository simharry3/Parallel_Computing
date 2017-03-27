#!/bin/sh

#SBATCH -p small
#SBATCH -O
#SBATCH -N 8
#SBATCH -t 00:60:00
#################################
#CHANGE THESE EVERY RUN:
#SBATCH -J 1-1-16-25:512
#SBATCH -o 1-1-16-25:512.dat
#################################
srun -n 512 ./assignment4.xl 1 1 16 25