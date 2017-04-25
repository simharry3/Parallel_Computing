#include "operations.h"


void stepSystem(state* st, context* ctx){
    for(int i = 0; i < st->activeParticles; ++i){
        updateParticlePosition(st, &(st->ptab[i]));
    }
}

void runSystem(state* st, context* ctx){
    for(int i = 0; i < ctx->numSteps; ++i){
        stepSystem(st, ctx);
    }
}