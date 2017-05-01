#include "operations.h"
#include <mpi.h>



void stepSystem(state* st, context* ctx){
    // printf("STEP %u\n", st->simSteps);
    fflush(NULL);
    updateParticlePositions(st, ctx);

}

void runSystem(state* st, context* ctx){
    int cont = 1;
    int temp = 1;
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
        while(cont == 1){
            MPI_Request request;
            fflush(NULL);
            stepSystem(st, ctx);
            ++st->simSteps;
            if(st->simSteps % ctx->chkFreq == 0){
                // if(ctx->rank == 0){
                // }
            }
            cont = 0;
            if(st->activeParticles > 0){
                for(int i = 0; i < ctx->comm_size; i++){
                    MPI_Request request2;
                    MPI_Isend(&temp, 1, MPI_INT, i, 234, MPI_COMM_WORLD, &request2);

                //MPI_Isend(st->ctab, st->collidedParticles, particle, i, 123, MPI_COMM_WORLD, &request);
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Irecv(&cont, 1, MPI_INT, MPI_ANY_SOURCE, 234, MPI_COMM_WORLD, &request);
            MPI_Barrier(MPI_COMM_WORLD);

        }
    }
    //MPI_Barrier(MPI_COMM_WORLD);
}

void printSimulationResults(state* st, context* ctx){
        printState(st, ctx);
        printf("Simulation finished in %u steps\n", st->simSteps);
        fflush(NULL);
}