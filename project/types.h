#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h>
#include <stdlib.h>

typedef double real;

typedef struct vec3D{
    real x, y, z;
}vec3D;

typedef struct particle{
    vec3D position;
    uint id;
    int collision;
    //ADD PARTICLE 
}particle;

typedef struct context{
    uint numParticles;
}context;

typedef struct state{
    particle* ptab;
    particle* ctab;
    //ADD STATE
}state;

void initParticle(particle* p, int id);
void printParticle(particle* p);

void initContext(context** ctx);
void initState(state** st, context* ctx);
void printState(state* st, context* ctx);
#endif /*TYPES_H_*/