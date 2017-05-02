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
        while(cont > 0){
            MPI_Request request, request2;
            fflush(NULL);
            stepSystem(st, ctx);
            ++st->simSteps;

            MPI_Barrier(MPI_COMM_WORLD);
            cont = 0;
            if(st->activeParticles > 0){
                temp = 1;
            }
            else{
                temp = 0;
            }
            for(int i = 0; i < ctx->comm_size; i++){
                // printf("SENDING %d\n", i);
                // fflush(NULL);
                MPI_Isend(&temp, 1, MPI_INT, i, 234, MPI_COMM_WORLD, &request2);
            }
            MPI_Barrier(MPI_COMM_WORLD);
            int tval = 0;
            for(int i = 0; i < ctx->comm_size; i++){
                // printf("RECEIVING %d\n", i);
                // fflush(NULL);
                MPI_Irecv(&cont, 1, MPI_INT, i, 234, MPI_COMM_WORLD, &request);
                tval += cont;
            }
            cont = tval;
            // printf("POOOORKKKKDDDD <<<<<<<<<< %d\n", cont);
        }
    }
    //MPI_Barrier(MPI_COMM_WORLD);
}



void printSimulationResults(state* st, context* ctx){
        if(ctx->rank == 0){
            printf("%u %f\n", st->simSteps, st->simTime);
            fflush(NULL);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        printState(st, ctx);
}