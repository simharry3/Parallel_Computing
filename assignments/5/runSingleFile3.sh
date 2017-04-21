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
#################################
srun -n 16 ./assignment5.xl 25 4 $1
