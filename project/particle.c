#include "particle.h"

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