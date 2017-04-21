#!/bin/sh
#########################
#       SIZING:
#------------------------
#       SMALL: 64
#       MEDIUM: 512
#########################


#SBATCH -p small  
#SBATCH -O
#SBATCH -N 1
#SBATCH -t 00:15:00
#################################
#CHANGE THESE EVERY RUN:
#SBATCH -J 1-64-0
#SBATCH -o 1-64-0.dat
#################################
srun -n 64 ./assignment5.xl 25 0 1
