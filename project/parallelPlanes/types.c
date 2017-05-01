#include <types.h>
#include <mpi.h>

int rank;
int numParts;
int temp = -1;

void initParticle(state* st, context* ctx, int* pos, int value){
    st->universe[0][pos[1] * ctx->max[0] + pos[0]] = value;
}

void printParticle(particle* p, int readability){
    if(readability == 1){
        printf("Particle %d: (%d, %d, %d) C: %d\n", p->id+(numParts*rank), p->x, 
                                     p->y, p->z, p->collision);
    }
    else{  
        printf("%d %d %d %d %d\n", p->id+(numParts*rank), p->collision, p->x, 
                                     p->y, p->z);
    }
    fflush(NULL);
}

//check if two particles have the same location:
int checkLocations(particle* p1, particle* p2){
    return p1->x == p2->x &&
            p1->y == p2->y &&
            p1->z == p2->z;
}

int checkParticleCollision(state* st, particle* p){
    int i;
    for(i = 0; i < st->collidedParticles; ++i){

        if(checkLocations(p, &(st->ctab[i]))){
            return 1;
        }
    }
    return 0;
}

void updateCollision(particle* cols, state* st, MPI_Datatype particle, context* ctx){
    MPI_Request request;
    // Currently set to always to rank 0 collision table.. in the future it
    // should run through all collions tables calls.. somehow.
    
    MPI_Irecv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 124, MPI_COMM_WORLD, &request);
   
    //st->ctab = realloc(st->ctab, st->collidedParticles * sizeof(particle));
    //MPI_Irecv(st->ctab, st->collidedParticles, particle, MPI_ANY_SOURCE, 123, MPI_COMM_WORLD, &request); 
    printf("\nAFTER RECEIVED !!!\n");
    printf("PRINTING COLLISION TABLE PASSED ON %d   \n", temp);
    printState(st, ctx);
    fflush(NULL);

 
}

void capParticle(context* ctx, particle* p){
    if(p->x > ctx->max[0]){
        p->x = ctx->max[0];
    }
    else if(p->x < 0){
        p->x = 0;
    }

    if(p->y > ctx->max[1]){
        p->y = ctx->max[1];
    }
    else if(p->y < 0){
        p->y = 0;
    }

    if(p->z > ctx->max[2]){
        p->z = ctx->max[2];
    }
    else if(p->z < 0){
        p->z = 0;
    }
}

void addCollidedParticle(state* st, particle* p){
    // if(rank == 0)
    //     printf("PID: %d\n", p->id);
    ++st->collidedParticles;
    // particle* tmp;
    st->ctab = realloc(st->ctab, st->collidedParticles * sizeof(particle));
    // free(st->ctab);
    // st->ctab = tmp;
    // tmp = NULL;

    (st->ctab[st->collidedParticles - 1]).x = p->x;
    (st->ctab[st->collidedParticles - 1]).y = p->y;
    (st->ctab[st->collidedParticles - 1]).z = p->z;
    (st->ctab[st->collidedParticles - 1]).collision = 1;
    (st->ctab[st->collidedParticles - 1]).id = p->id;
    // if(rank == 0){
    //     printf("COLLIDED PARTICLE INFORMATION:\n");
    //     printParticle(&(st->ctab[st->collidedParticles - 1]));
    // }
    
    fflush(NULL);

    //Scan the list of bodies (up until the end), move the particle we want to remove to the end:
    if(st->activeParticles > 1){
        int i;
        for( i = 0;  i < st->activeParticles - 1; ++i){
            if(&(st->ptab[i]) == p){
                st->ptab[i] = st->ptab[st->activeParticles - 1];
            }
        }
        --st->activeParticles;
        st->ptab = realloc(st->ptab, st->activeParticles * sizeof(particle));
            // free(st->ptab);
            // st->ptab = tmp;
            // tmp = NULL;
    }
    else{
        --st->activeParticles;
    }
}



void updateParticlePositions(state* st, context* ctx){
    MPI_Request request;
    int num;
    int dx, dy, dz;
    dx = dy = dz = 0;
    int* pos = calloc(3, sizeof(int));
    for(int i = 0; i < ctx->planesPerRank; ++i){
        for(int j = 0; j < ctx->max[0] * ctx->max[1]; ++j){
            pos[0] = j % ctx->max[0];
            pos[1] =  (j - pos[0]) / ctx->max[1];
            pos[2] = i;
            // printf("%d %d %d\n", pos[0], pos[1], pos[2]);
            num = rand() % 6;
            switch(num){
                case 0:
                    dx = -1;
                    break;
                case 1:
                    dx = 1;
                    break;
                case 2:
                    dy = -1;
                    break;
                case 3:
                    dy = 1;
                    break;
                case 4:
                    dz = -1;
                    break;
                case 5:
                    dz = 1;
                    break;
            }
            pos[0] += dx;
            pos[1] += dy;
            pos[2] += dz;
        }
    }

    //MPI_Barrier(MPI_COMM_WORLD);
    // updateCollision(st->ctab, st, ctx);
    // if(!checkParticleCollision(st, p)){
    // //Check for collision here
    //     capParticle(ctx, p);
    // }
    // else{
    //     p->x -= dx;
    //     p->y -= dy;
    //     p->z -= dz;
    //     //printParticle(p);
    //     addCollidedParticle(st, p);
    //     int i;
    //     for(i = 0; i < ctx->comm_size; i++)
    //         if(i != my_rank){
    //             MPI_Isend(&(st->collidedParticles), 1, MPI_INT, i, 124, MPI_COMM_WORLD, &request);

    //             //MPI_Isend(st->ctab, st->collidedParticles, particle, i, 123, MPI_COMM_WORLD, &request);
    //         }

        //MPI_Bcast((void *)&(st->collidedParticles), 1, MPI_INT, my_rank, MPI_COMM_WORLD);
        //MPI_Bcast(st->ctab, st->collidedParticles, particle, my_rank, MPI_COMM_WORLD);

    // }
    //printParticle(p);

    //Check for collision here
    // if collision call MPI_Isend
}




void initContext(context** ctx, int* data){
    *ctx = calloc(1, sizeof(context));

    MPI_Comm_size(MPI_COMM_WORLD, &((*ctx)->comm_size));
    MPI_Comm_rank(MPI_COMM_WORLD, &((*ctx)->rank));

    (*ctx)->max = calloc(3, sizeof(int));
    (*ctx)->numParticles = data[0];

    (*ctx)->numSteps = data[1];
    (*ctx)->chkFreq = data[2];
    (*ctx)->humanOutput = data[3];

    (*ctx)->max[0] = (*ctx)->max[1] = (*ctx)->max[2] = 16;
    

    (*ctx)->planesPerRank = (*ctx)->max[2]/(*ctx)->comm_size;
    (*ctx)->particlesPerRank = (*ctx)->numParticles/(*ctx)->comm_size;
    
}

void initState(state** st, context* ctx){
    *st = calloc(1, sizeof(state));
    int* pos = (int*)calloc(3, sizeof(int));
    
    //Random Variable Initialization
    time_t t;
    srand((unsigned) time(&t));
    
    size_t planeMalloc = ctx->planesPerRank * sizeof(char*);
    size_t innerPlaneMalloc = ctx->max[0]  * ctx->max[1] * sizeof(char);
    if(ctx->rank == 0){
        printf("Beginning allocation of %u planes of size %zu\n", ctx->planesPerRank, innerPlaneMalloc);
        fflush(NULL);
    }
    //We use a double pointer that is a representation of planes of our simulation. This is a compromise for 
    //Memory conservation's sake:
    (*st)->universe = malloc(ctx->planesPerRank * sizeof(char*));
    for(int i = 0; i < ctx->planesPerRank; ++i){
        (*st)->universe[i] = calloc(ctx->max[0] * ctx->max[1], sizeof(char));
    }

    if(ctx->rank == 0){
        printf("ALLOCATION COMPLETE\n");
        fflush(NULL);
    }

    int i;

    for(i = 0; i < ctx->numParticles; ++i){
        pos[0] = rand() % ctx->max[0];
        pos[1] = rand() % ctx->max[1];
        pos[2] = rand() % ctx->max[2];
        initParticle(*st, ctx, pos, ACTIVE_PARTICLE);
    }

    (*st)->activeParticles = ctx->numParticles;
    (*st)->collidedParticles = 0;
    (*st)->simSteps = 0;
    //printState(*st, ctx);
}

void destroyState(state** st, context* ctx){
    for(int i = 0; i < ctx->planesPerRank; ++i){
        free((*st)->universe[i]);
    }

   free((*st)->universe);
   free((*st));
   st = NULL;
}


void initAggregators(state* st, context* ctx, char* agFile){
    
    // if(my_rank == 0)
    //     printf("READING AG DATA FROM FILE: %s\n", agFile);
    
    int* pos = (int*)calloc(3, sizeof(int));
    char* buff = (char*)calloc(8, sizeof(char));
    FILE* fp;
    fp = fopen(agFile, "r");
    while(fscanf(fp, "%d %d %d", &pos[0], &pos[1], &pos[2]) != EOF){
        // st->ctab = realloc(st->ctab, (st->collidedParticles + 1) * sizeof(particle));
        initParticle(st, ctx, pos, COLLIDED_PARTICLE);
        // ++st->collidedParticles;
    }
    free(pos);
    free(buff);

    //printf("READING AGGREGATORS \n");
}


void printState(state* st, context* ctx){
    // printf("ACTIVE PARTICLES:\n =========================\n");
    int i;
    for(i = 0; i < st->activeParticles; ++i){
        printParticle(&(st->ptab[i]), ctx->humanOutput);
    }
    for(i = 0; i < st->collidedParticles; ++i){
        printf("HERE");
        printParticle(&(st->ctab[i]), ctx->humanOutput);
        //printf("%d\n", st->collidedParticles);
    }
}