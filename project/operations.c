#include "operations.h"


void stepSystem(state* st, context* ctx){
    for(int i = 0; i < ctx->numParticles; ++i){
        updateParticlePosition(&(st->ptab[i]));
    }
}

void runSystem(state* st, context* ctx){
    for(int i = 0; i < ctx->numSteps; ++i){
        stepSystem(st, ctx);
    }
}