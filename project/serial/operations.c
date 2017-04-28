#include "operations.h"


void stepSystem(state* st, context* ctx){
    // printf("STEP %u\n", st->simSteps);
    fflush(NULL);
    for(int i = 0; i < st->activeParticles; ++i){
        uint collisionCheck = st->activeParticles;
        updateParticlePosition(st, ctx, &(st->ptab[i]));
        if(collisionCheck != st->activeParticles){
            printf("STEP %u: COLLISION DETECTED AND ACCOUNTED FOR\n", st->simSteps);
            --i;
        }
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