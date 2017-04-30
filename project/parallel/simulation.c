#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "types.h"
#include "operations.h"


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
    int i;
    for(i = 1; i < argc; ++i){
        data[i - 1] = strtol(argv[i], NULL, 10);
    }

    
    initContext(&ctx, data);
    initState(&st, ctx);
    initAggregators(st, argv[argc-1]);
    printState(st, ctx);
    runSystem(st, ctx, mpi_rank);
    printState(st, ctx);
    printSimulationResults(st, ctx);
    exit(0);
}

