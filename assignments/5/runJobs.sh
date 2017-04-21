#!/bin/sh
sbatch -o 1-64-0.dat -J 1-64-0 -N 1 runSingleFile1.sh 1
sbatch -o 1-8-8.dat -J 1-8-8 -N 1 runSingleFile2.sh 1
sbatch -o 1-16-4.dat -J 1-16-4 -N 1 runSingleFile3.sh 1

sbatch -o 4-64-0.dat -J 4-64-0 -N 4 runSingleFile1.sh 4
sbatch -o 4-8-8.dat -J 4-8-8 -N 4 runSingleFile2.sh 4
sbatch -o 4-16-4.dat -J 4-16-4 -N 4 runSingleFile3.sh 4

sbatch -o 16-64-0.dat -J 16-64-0 -N 16 runSingleFile1.sh 16
sbatch -o 16-8-8.dat -J 16-8-8 -N 16 runSingleFile2.sh 16
sbatch -o 16-16-4.dat -J 16-16-4 -N 16 runSingleFile3.sh 16

sbatch -o 64-64-0.dat -J 64-64-0 -N 64 runSingleFile1.sh 64
sbatch -o 64-8-8.dat -J 64-8-8 -N 64 runSingleFile2.sh 64
sbatch -o 64-16-4.dat -J 64-16-4 -N 64 runSingleFile3.sh 64
