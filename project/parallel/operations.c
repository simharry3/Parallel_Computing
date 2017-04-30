#include "operations.h"
#include <mpi.h>



void stepSystem(state* st, context* ctx, int mpi_rank){
    // printf("STEP %u\n", st->simSteps);
    fflush(NULL);
    int i;
    for(i = 0; i < st->activeParticles; ++i){
    	MPI_Barrier(MPI_COMM_WORLD);
        uint collisionCheck = st->activeParticles;
        updateParticlePosition(st, ctx, &(st->ptab[i]), mpi_rank);
        if(collisionCheck != st->activeParticles){
            printf("STEP %u: COLLISION DETECTED AND ACCOUNTED FOR\n", st->simSteps);
            --i;
        }
    }
}

void runSystem(state* st, context* ctx, int mpi_rank){
    MPI_Barrier(MPI_COMM_WORLD);
    //printf("%d\n", st->activeParticles);
    if(ctx->numSteps > 1){
    	int i;
        for(i = 0; i < ctx->numSteps; ++i){
            stepSystem(st, ctx, mpi_rank);
        }
    }
    else{
        while(st->activeParticles > 0){
            fflush(NULL);
            stepSystem(st, ctx, mpi_rank);
            ++st->simSteps;
        }
    }
}

void printSimulationResults(state* st, context* ctx){
    printf("Simulation finished in %u steps\n", st->simSteps);
}