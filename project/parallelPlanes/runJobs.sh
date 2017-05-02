#!/bin/sh
sbatch -o 2-16-2048.dat -J 1-1024-8192 -N 1 runSingleFile1.sh 2 2048 16 -1
sbatch -o 4-16-2048.dat -J 2-1024-8192 -N 1 runSingleFile1.sh 4 2048 16 -1
sbatch -o 8-16-2048.dat -J 4-1024-8192 -N 4 runSingleFile1.sh 8 2048 16 -1
sbatch -o 16-16-2048.dat -J 8-1024-8192 -N 8 runSingleFile1.sh 16 2048 16 -1
