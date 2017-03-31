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
    uint numParticles;
    particle* ptab;
    particle* ctab;
    //ADD STATE
}state;

void initParticle(particle* p);
void printParticle(particle* p);

void initState(state* st);
void printState(state* st);
#endif /*TYPES_H_*/