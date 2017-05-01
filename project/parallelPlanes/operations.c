#include "operations.h"
#include <mpi.h>



void stepSystem(state* st, context* ctx){
    // printf("STEP %u\n", st->simSteps);
    fflush(NULL);
    updateParticlePositions(st, ctx);
    // for(i = 0; i < st->activeParticles; ++i){
    // 	//MPI_Barrier(MPI_COMM_WORLD);
    //     uint collisionCheck = st->activeParticles;
    //     updateParticlePosition(st, ctx);
    //     //printf("STEP SYSTEM STUCK HERE %d. %d RANK \n", i, mpi_rank);
    //     fflush(NULL);
    //     if(collisionCheck != st->activeParticles){
    //         //printf("HERE\n");
    //         //printf("STEP %u: COLLISION DETECTED AND ACCOUNTED FOR\n", st->simSteps);

    //         --i;
    //         //printf("%d is the index checked, %d is the number of activeParticles\n", i, st->activeParticles);
    //     }
    // }
    //printf("FINSHED STEP SYSTEM, %d active particle count\n", st->activeParticles);
}

void runSystem(state* st, context* ctx){
    MPI_Barrier(MPI_COMM_WORLD);
    //printf("%d\n", st->activeParticles);
    if(ctx->numSteps > 0){
    	int i;
        for(i = 0; i < ctx->numSteps; ++i){
            stepSystem(st, ctx);
            ++st->simSteps;
            // //printf("RUN SYSTEM STUCK HERE %d, %d RANK \n", i, mpi_rank);
            // sleep(1);
            fflush(NULL);
        }
    }
    else{
        while(st->activeParticles > 0){
            fflush(NULL);
            stepSystem(st, ctx);
            ++st->simSteps;
            if(st->simSteps % ctx->chkFreq == 0){
                if(ctx->rank == 0){
                    // printf("%u\n", st->activeParticles);
                    // printf("=====STEP %d:=====\n", st->simSteps);
                    // printState(st, ctx);
                }
                //MPI_Barrier(MPI_COMM_WORLD);
            }
            
        }
    }
    //MPI_Barrier(MPI_COMM_WORLD);
}

void printSimulationResults(state* st, context* ctx){
    printState(st, ctx);
    printf("Simulation finished in %u steps\n", st->simSteps);
    fflush(NULL);
}