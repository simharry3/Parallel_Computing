#!/bin/sh
sbatch -o 1-1024-8192.dat -J 1-1024-8192 -N 1 runSingleFile1.sh 32 8192 1024 -1 0 0
sbatch -o 2-1024-8192.dat -J 2-1024-8192 -N 2 runSingleFile1.sh 64 8192 1024 -1 0 0
sbatch -o 4-1024-8192.dat -J 4-1024-8192 -N 4 runSingleFile1.sh 128 8192 1024 -1 0 0
sbatch -o 8-1024-8192.dat -J 8-1024-8192 -N 8 runSingleFile1.sh 256 8192 1024 -1 0 0
sbatch -o 16-1024-8192.dat -J 16-1024-8192 -N 16 runSingleFile1.sh 512 8192 1024 -1 0 0
sbatch -o 32-1024-8192.dat -J 32-1024-8192 -N 32 runSingleFile1.sh 1024 8192 1024 -1 0 0
