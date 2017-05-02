#!/bin/sh
sbatch -o 1-64-8192.dat -J 1-64-8192 -N 1 runSingleFile1.sh 64 8192 64 -1 0 0 'test1'