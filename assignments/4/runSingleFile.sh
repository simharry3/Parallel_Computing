#!/bin/sh
#########################
#       SIZING:
#------------------------
#       SMALL: 64
#       MEDIUM: 512
#########################


#SBATCH -p medium 
#SBATCH -O
#SBATCH -N 128
#SBATCH -t 00:60:00
#################################
#CHANGE THESE EVERY RUN:
#SBATCH -J 1-2-16-25:8192
#SBATCH -o 1-2-16-25:8192.dat
#################################
srun -n 8192 ./assignment4.xl 1 2 16 25
