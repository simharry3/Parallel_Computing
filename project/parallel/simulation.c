#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "operations.h"
#include <mpi.h>

int main(int argc, char* argv[]){
    printf("HERE WE GO\n");
    state* st;
    context* ctx;

    int mpi_rank;
    int mpi_commsize;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    //<NUM PARTICLES> <NUM STEPS>
    int* data = calloc(argc - 1, sizeof(int));
    for(int i = 1; i < argc; ++i){
        data[i - 1] = strtol(argv[i], NULL, 10);
    }

    

    initContext(&ctx, data, mpi_commsize);
    initState(&st, ctx);

    printState(st, ctx, data[0]);
    runSystem(st, ctx);
    printState(st, ctx);
    exit(0);
}