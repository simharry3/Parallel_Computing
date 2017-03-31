#include <types.h>


void initParticle(particle* p, int id){
    p->position.x = 0;
    p->position.y = 0;
    p->position.z = 0;

    p->id = id;
}

void printParticle(particle* p){
    printf("Particle %d: %f, %f, %f\n", p->id, p->position.x, 
                                     p->position.y, p->position.z);
}


void initContext(context** ctx){
    *ctx = calloc(1, sizeof(context));
    (*ctx)->numParticles = 10;
}

void initState(state** st, context* ctx){
    *st = calloc(1, sizeof(state));
    (*st)->ptab = calloc(ctx->numParticles, sizeof(particle));
    for(int i = 0; i < ctx->numParticles; ++i){
       initParticle(&(((*st)->ptab)[i]), i);
    }
}

void printState(state* st, context* ctx){
    for(int i = 0; i < ctx->numParticles; ++i){
        printParticle(&(st->ptab[i]));
    }
}