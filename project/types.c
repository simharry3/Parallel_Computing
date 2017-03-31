#include <types.h>


void initParticle(particle* p, int* pos, int id){
    p->position.x = rand() % pos[0];
    p->position.y = rand() % pos[1];
    p->position.z = rand() % pos[2];

    p->id = id;
}

void printParticle(particle* p){
    printf("Particle %d: %f, %f, %f\n", p->id, p->position.x, 
                                     p->position.y, p->position.z);
}


void initContext(context** ctx){
    *ctx = calloc(1, sizeof(context));
    (*ctx)->max = calloc(3, sizeof(int));
    (*ctx)->numParticles = 10;
    (*ctx)->max[0] = 100;
    (*ctx)->max[1] = 100;
    (*ctx)->max[2] = 100;
}

void initState(state** st, context* ctx){
    *st = calloc(1, sizeof(state));
    int* pos = (int*)calloc(3, sizeof(int));time_t t;

    srand((unsigned) time(&t));

    (*st)->ptab = calloc(ctx->numParticles, sizeof(particle));
    for(int i = 0; i < ctx->numParticles; ++i){
        pos[0] = rand() % ctx->max[0];
        pos[1] = rand() % ctx->max[1];
        pos[2] = rand() % ctx->max[2];
        initParticle(&(((*st)->ptab)[i]), pos, i);
    }
}

void printState(state* st, context* ctx){
    for(int i = 0; i < ctx->numParticles; ++i){
        printParticle(&(st->ptab[i]));
    }
}