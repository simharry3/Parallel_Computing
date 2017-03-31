#include <types.h>


void initParticle(particle* p){
    p->position.x = 0;
    p->position.y = 0;
    p->position.z = 0;

    p->id = 0;
}

void printStatus(particle* p){
    printf("Particle %d: %f, %f, %f\n", p->id, p->position.x, 
                                     p->position.y, p->position.z);
}


void initContext(context* ctx){
    ctx->numParticles = 10;
}
void initState(state* st, context* ctx){
    st->ptab = calloc(ctx->numParticles, sizeof(particle));
    for(int i = 0; i < ctx->numParticles; ++i){
       initParticle(&((st->ptab)[i]));
    }
}