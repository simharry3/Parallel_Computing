#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Needed for memcpy
#include <time.h> //Needed for RNG 
#include <mpi.h>

#define EMPTY_CELL 0
#define ACTIVE_PARTICLE 1
#define AGGREGATOR_PARTICLE 9
#define COLLIDED_PARTICLE 10
#define CELL_MAX 6

typedef double real;

typedef struct vec3D{
    int x, y, z;
}vec3D;

typedef struct particle{
    int x;
    int y;
    int z;
    int id;
    int collision;
    //ADD PARTICLE 
}particle;

typedef struct context{
    uint numParticles;
    uint particlesPerRank;
    uint planesPerRank;
    int comm_size;
    int rank;

    int numSteps;
    int chkFreq;
    int humanOutput;
    
    int* max;
    char* checkpointFile;
}context;

typedef struct state{
    uint activeParticles;
    uint collidedParticles;
    uint simSteps;
    
    particle* ptab;
    particle* ctab;

    char** universe;
    //ADD STATE
}state;

void initParticle(state* st, context* ctx, int* pos, int value);
void printParticle(particle* p, int readability);
void updateParticlePosition(state* st, context* ctx);
void updateCollision(particle* cols, state* st, MPI_Datatype particle, context* ctx);
void initAggregators(state* st, context* ctx, char* agFile);
void initContext(context** ctx, int* data);
void initState(state** st, context* ctx);
void destroyState(state** st, context* ctx);
void printState(state* st, context* ctx);

#endif /*TYPES_H_*/
