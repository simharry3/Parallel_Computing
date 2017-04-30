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
            //printf("HERE\n");
            //printf("STEP %u: COLLISION DETECTED AND ACCOUNTED FOR\n", st->simSteps);

            --i;
            //printf("%d is the index checked, %d is the number of activeParticles\n", i, st->activeParticles);
        }
    }
    //printf("FINSHED STEP SYSTEM, %d active particle count\n", st->activeParticles);
}

void runSystem(state* st, context* ctx, int mpi_rank){
    MPI_Barrier(MPI_COMM_WORLD);
    //printf("%d\n", st->activeParticles);
    if(ctx->numSteps > 1){
    	int i;
        for(i = 0; i < ctx->numSteps; ++i){
            stepSystem(st, ctx, mpi_rank);
            ++st->simSteps;
        }
    }
    else{
        while(st->activeParticles > 0){
            fflush(NULL);
            stepSystem(st, ctx, mpi_rank);
            ++st->simSteps;
            if(st->simSteps % ctx->chkFreq == 0){
                if(mpi_rank == 0){
                    printf("=====STEP %d:=====\n", st->simSteps);
                    printState(st, ctx);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
            //printf("STUCK HERE\n");
        }
    }
}

void printSimulationResults(state* st, context* ctx){
    printState(st, ctx);
    printf("Simulation finished in %u steps\n", st->simSteps);
    fflush(NULL);
}