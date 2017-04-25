#include "operations.h"
#include <mpi.h>


void stepSystem(state* st, context* ctx, int commsize, int numAll){
    for(int i = 0; i < ctx->numParticles; ++i){
    	updateCollisoins(&(st->ctab), numAll);
    	MPI_Barrier(MPI_COMM_WORLD);
        updateParticlePosition(&(st->ptab[i]));
    }
}

void runSystem(state* st, context* ctx, int commsize, int numAll){
    for(int i = 0; i < ctx->numSteps; ++i){
        stepSystem(st, ctx, commsize, numAll);
    }
}