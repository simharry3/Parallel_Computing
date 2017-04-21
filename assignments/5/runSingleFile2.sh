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
srun -n 8 ./assignment5.xl 25 8 $1
