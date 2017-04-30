#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Needed for memcpy
#include <time.h> //Needed for RNG 

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
    uint numSteps;
    int* max;
    char* checkpointFile;
}context;

typedef struct state{
    uint activeParticles;
    uint collidedParticles;
    uint simSteps;
    
    particle* ptab;
    particle* ctab;
    //ADD STATE
}state;

void initParticle(particle* p, int* pos, int id);
void printParticle(particle* p);
void updateParticlePosition(state* st, context* ctx, particle* p, int my_rank);
void updateCollision(particle* cols);
void initAggregators(state* st, char* agFile);
void initContext(context** ctx, int* data);
void initState(state** st, context* ctx);
void printState(state* st, context* ctx);

#endif /*TYPES_H_*/
