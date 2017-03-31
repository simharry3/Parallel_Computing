#!/bin/sh
#SBATCH --job-name TESTING
#SBATCH -partition medium
#SBATCH -t 04:00:00
#SBATCH -D /gpfs/u/barn/3/
#
#SBATCH --mail-type ALL
#SBATCH --mail-user simharry3@gmail.com

srun  --time 60 --ntasks 32 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 64 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 128 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 256 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 512 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 1024 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 2048 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 4096 --ntasks-per-node 64 --output runtimes.txt ./assignment3 0
srun  --time 60 --ntasks 8192 --ntasks-per-node 64 -- output runtimes.txt ./assignment3 0

srun  --time 60 --ntasks 32 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 64 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 128 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 256 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 512 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 1024 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 2048 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 4096 --ntasks-per-node 64 --output runtimes.txt ./assignment3 25
srun  --time 60 --ntasks 8192 --ntasks-per-node 64 -- output runtimes.txt ./assignment3 25

srun  --time 60 --ntasks 32 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 64 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 128 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 256 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 512 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 1024 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 2048 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 4096 --ntasks-per-node 64 --output runtimes.txt ./assignment3 50
srun  --time 60 --ntasks 8192 --ntasks-per-node 64 -- output runtimes.txt ./assignment3 50

srun  --time 60 --ntasks 32 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 64 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 128 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 256 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 512 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 1024 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 2048 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 4096 --ntasks-per-node 64 --output runtimes.txt ./assignment3 75
srun  --time 60 --ntasks 8192 --ntasks-per-node 64 -- output runtimes.txt ./assignment3 75

srun  --time 60 --ntasks 32 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50
srun  --time 60 --ntasks 64 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50
srun  --time 60 --ntasks 128 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50
srun  --time 60 --ntasks 256 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50
srun  --time 60 --ntasks 512 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50
srun  --time 60 --ntasks 1024 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50
srun  --time 60 --ntasks 2048 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50
srun  --time 60 --ntasks 4096 --ntasks-per-node 32 --output runtimes32.txt ./assignment3 50