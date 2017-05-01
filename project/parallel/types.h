#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Needed for memcpy
#include <time.h> //Needed for RNG 
#include <mpi.h>

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
    uint comm_size;
    particle* ptab;
    particle* ctab;
    //ADD STATE
}state;

void initParticle(particle* p, int* pos, int id, int collision);
void printParticle(particle* p, int readability);
void updateParticlePosition(state* st, context* ctx, particle* p, int my_rank, MPI_Datatype particle);
void updateCollision(particle* cols, state* st, MPI_Datatype particle, context* ctx);
void initAggregators(state* st, char* agFile);
void initContext(context** ctx, int* data, int comm_size);
void initState(state** st, context* ctx, int my_rank, int size);
void printState(state* st, context* ctx);

#endif /*TYPES_H_*/
