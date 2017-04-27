#include "operations.h"


void stepSystem(state* st, context* ctx){
    for(int i = 0; i < st->activeParticles; ++i){
        updateParticlePosition(st, ctx, &(st->ptab[i]));
    }
}

void runSystem(state* st, context* ctx){
    if(ctx->numSteps > 1){
        for(int i = 0; i < ctx->numSteps; ++i){
            stepSystem(st, ctx);
        }
    }
    else{
        while(st->activeParticles > 0){
            fflush(NULL);
            stepSystem(st, ctx);
            ++st->simSteps;
        }
    }
}

void printSimulationResults(state* st, context* ctx){
    printf("Simulation finished in %u steps\n", st->simSteps);
}