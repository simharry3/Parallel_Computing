#include <types.h>
#include <mpi.h>

int rank;
int numParts;
int temp = -1;

void initParticle(state* st, context* ctx, int* pos, int value){
    // if(ctx->rank == 0){
    //     printf("POS: %d\n", pos[2]);
    //     fflush(NULL);
    // }
    st->universe[pos[2]][pos[1] * ctx->max[0] + pos[0]] = value;
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

int checkParticleCollision(state* st, context* ctx, int* pos){
    return  st->universe[pos[2]][pos[1] * ctx->max[0] + pos[0]];
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

void capPosition(context* ctx, int** pos){
    if((*pos)[0] > ctx->max[0] - 1){
        (*pos)[0] = ctx->max[0] - 1;
    }
    else if((*pos)[0] < 0){
        (*pos)[0] = 0;
    }

    if((*pos)[1] > ctx->max[1] - 1){
        (*pos)[1] = ctx->max[1] - 1;
    }
    else if((*pos)[1] < 0){
        (*pos)[1] = 0;
    }

    if((*pos)[2] > ctx->planesPerRank - 1){
        (*pos)[2] = ctx->planesPerRank - 1;
    }
    else if((*pos)[2] < 0){
        (*pos)[2] = 0;
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
int wrapRank(state* st, context* ctx, int rankIn){
    if(rankIn > ctx->comm_size - 1){
        return 0;
    }
    if(rankIn < 0){
        return ctx->comm_size - 1;
    }
    return rankIn;
}

void updateGhostRows(state* st, context* ctx){
    MPI_Request send1, send2;
    MPI_Request recv1, recv2;
    int* buff1 = calloc(ctx->max[0] * ctx->max[1], sizeof(int));
    int* buff2 = calloc(ctx->max[0] * ctx->max[1], sizeof(int));

    // printf("RANK %d FORWARD RANK: %d BACKWARD RANK: %d \n",
    //         ctx->rank, wrapRank(st, ctx, ctx->rank  + 1), wrapRank(st, ctx, ctx->rank - 1));

    MPI_Isend((st->universe[0]), ctx->max[0] * ctx->max[1], 
            MPI_INT, wrapRank(st, ctx, ctx->rank  - 1), 1, MPI_COMM_WORLD, &send1);
    MPI_Isend((st->universe[ctx->planesPerRank + 1]), ctx->max[0] * ctx->max[1], 
            MPI_INT, wrapRank(st, ctx, ctx->rank  + 1), 1, MPI_COMM_WORLD, &send2);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Irecv(buff1, ctx->max[0] * ctx->max[1], 
            MPI_INT, wrapRank(st, ctx, ctx->rank  - 1), 1, MPI_COMM_WORLD, &recv1);
    MPI_Irecv(buff2, ctx->max[0] * ctx->max[1], 
            MPI_INT, wrapRank(st, ctx, ctx->rank  + 1), 1, MPI_COMM_WORLD, &recv2);
    printf("=============================\n");
    for(int i = 0; i < ctx->max[0] * ctx->max[1]; ++i){
        printf("%d ", buff1[i]);
        if(st->universe[1][i] >= EMPTY_CELL){
            st->universe[1][i] += buff1[i];
        }
        if(st->universe[ctx->planesPerRank][i] >= EMPTY_CELL){
            st->universe[ctx->planesPerRank][i] += buff2[i];
        }

        st->universe[0][i] = EMPTY_CELL;
        st->universe[ctx->planesPerRank + 1][i] = EMPTY_CELL;
    }
    printf("\n=============================\n");
}


void updateParticlePositions(state* st, context* ctx){
    MPI_Request request;
    int num;
    int* d = calloc(3, sizeof(int));
    d[0] = d[1] = d[2] = 0;
    int** moveList = NULL;
    int movedParticles = 0;
    int* pos = calloc(3, sizeof(int));

   updateGhostRows(st, ctx);

    for(int i = 1; i < ctx->planesPerRank + 1; ++i){
        for(int j = 0; j < ctx->max[0] * ctx->max[1]; ++j){
            if(st->universe[i][j] >= ACTIVE_PARTICLE && st->universe[i][j] < ACTIVE_PARTICLE + CELL_MAX){
                pos[0] = j % ctx->max[0];
                pos[1] =  (j - pos[0]) / ctx->max[1];
                pos[2] = i;
                if(ctx->rank == 0){
                    printf("%d %d %d %d\n", pos[0], pos[1], pos[2], st->universe[pos[2]][pos[1] * ctx->max[0] + pos[0]]);
                    fflush(NULL);
                }
                int particlesOnTile = st->universe[i][j] - (ACTIVE_PARTICLE - 1);
                st->universe[i][j] = EMPTY_CELL;
                for(int g = 0; g < particlesOnTile; ++g){
                    num = rand() % 6;
                    switch(num){
                        case 0:
                            d[0] = -1;
                            break;
                        case 1:
                            d[0] = 1;
                            break;
                        case 2:
                            d[1] = -1;
                            break;
                        case 3:
                            d[1] = 1;
                            break;
                        case 4:
                            d[2] = -1;
                            break;
                        case 5:
                            d[2] = 1;
                            break;
                    }
                    pos[0] += d[0];
                    pos[1] += d[1];
                    pos[2] += d[2];
                    capPosition(ctx, &pos);
                    int checkedValue = checkParticleCollision(st, ctx, pos);
                    if(checkedValue == AGGREGATOR_PARTICLE || checkedValue == COLLIDED_PARTICLE){
                        st->universe[i][j] = COLLIDED_PARTICLE;
                        // printf("HERE<<<<<<<<<<<<<<<<<<<<<<<\n");
                        fflush(NULL);
                    }
                    else if(checkedValue < ACTIVE_PARTICLE + CELL_MAX){
                        moveList = (int**)realloc(moveList, (movedParticles + 1) * sizeof(int*));
                        moveList[movedParticles] = calloc(4, sizeof(int));

                        for(int k = 0; k < 3; ++k){
                            moveList[movedParticles][k] = pos[k];
                        }

                        ++movedParticles;
                    }
                    else{
                        --j;
                    }
                }
            }
            // CHECK NEIGHBORS
            // MOVE PARTICLE
        }
    }

    st->activeParticles = movedParticles;
    for(int i = 0; i < movedParticles; ++i){
        if(st->universe[moveList[i][2]][moveList[i][1] * ctx->max[0] + moveList[i][0]] == EMPTY_CELL){
            st->universe[moveList[i][2]][moveList[i][1] * ctx->max[0] + moveList[i][0]] = ACTIVE_PARTICLE;
        }
        else{
            st->universe[moveList[i][2]][moveList[i][1] * ctx->max[0] + moveList[i][0]] += 1;
        }
        free(moveList[i]);
    }
    free(moveList);
    free(pos);
    free(d);
    if(ctx->rank == 0){
        printf("====== %u =======\n", st->activeParticles);
        fflush(NULL);
    }
    // sleep(1);
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

    (*ctx)->max[0] = (*ctx)->max[1] = (*ctx)->max[2] = 4;
    

    (*ctx)->planesPerRank = (*ctx)->max[2]/(*ctx)->comm_size;
    (*ctx)->particlesPerRank = (*ctx)->numParticles/(*ctx)->comm_size;
    
}

void initState(state** st, context* ctx){
    *st = calloc(1, sizeof(state));
    int* pos = (int*)calloc(3, sizeof(int));
    
    size_t planeMalloc = ctx->planesPerRank * sizeof(int*);
    size_t innerPlaneMalloc = ctx->max[0]  * ctx->max[1] * sizeof(int);
    if(ctx->rank == 0){
        // printf("Beginning allocation of %u planes of size %zu\n", ctx->planesPerRank, innerPlaneMalloc);
        // fflush(NULL);
    }
    //We use a double pointer that is a representation of planes of our simulation. This is a compromise for 
    //Memory conservation's sake:
    (*st)->universe = malloc((ctx->planesPerRank + 2) * sizeof(int*));
    for(int i = 0; i < ctx->planesPerRank + 2; ++i){
        (*st)->universe[i] = calloc(ctx->max[0] * ctx->max[1], sizeof(int));
        memset((*st)->universe[i], EMPTY_CELL, ctx->max[0] * ctx->max[1] * sizeof(int));
    }

    if(ctx->rank == 0){
        // printf("ALLOCATION COMPLETE\n");
        // fflush(NULL);
    }

    int i;

    for(i = 0; i < ctx->particlesPerRank; ++i){
        pos[0] = rand() % ctx->max[0];
        pos[1] = rand() % ctx->max[1];
        pos[2] = (rand() % ctx->planesPerRank) + 1;
        // printf("ID: %d, CHECKING %d %d %d\n", i, pos[0], pos[1], pos[2]);
        // fflush(NULL);
        int checkVal = (*st)->universe[pos[2]][pos[1] * ctx->max[0] + pos[0]];
        if(checkVal <= ACTIVE_PARTICLE + CELL_MAX){
            // printf("ID: %d, PLACING PARTICLE\n", i);
            // fflush(NULL);
            if(checkVal == 0){
                initParticle(*st, ctx, pos, ACTIVE_PARTICLE);
            }
            else{
                initParticle(*st, ctx, pos, checkVal + 1);
            }
        }
        else{
            // printf("ID: %d, OVERLAP\n", i);
            // fflush(NULL);
            --i;
        }
    }

    (*st)->activeParticles = ctx->particlesPerRank;
    //  printf("PPR: %u\n", (*st)->activeParticles);
    (*st)->collidedParticles = 0;
    (*st)->simSteps = 0;
    free(pos);
    //printState(*st, ctx);
}

void destroyState(state** st, context* ctx){
    for(int i = 0; i < ctx->planesPerRank + 2; ++i){
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
    int* buff = (int*)calloc(8, sizeof(int));
    FILE* fp;
    fp = fopen(agFile, "r");
    while(fscanf(fp, "%d %d %d", &pos[0], &pos[1], &pos[2]) != EOF){
        pos[2];
        st->ctab = realloc(st->ctab, (st->collidedParticles + 1) * sizeof(particle));
        if(ctx->rank == pos[2]/ctx->planesPerRank){
            initParticle(st, ctx, pos, AGGREGATOR_PARTICLE);
        }
        // ++st->collidedParticles;
    }
    free(pos);
    free(buff);

    //printf("READING AGGREGATORS \n");
}


void printState(state* st, context* ctx){
   int* pos = calloc(3, sizeof(int));
    for(int i = 1; i < ctx->planesPerRank + 1; ++i){
        // printf("%d\n", i);
        for(int j = 0; j < ctx->max[0] * ctx->max[1]; ++j){
            if(st->universe[i][j] < EMPTY_CELL){
                pos[0] = j % ctx->max[0];
                pos[1] =  (j - pos[0]) / ctx->max[1];
                pos[2] = ctx->rank * ctx->planesPerRank + i;
                printf("%d %d %d %d\n", pos[0], pos[1], pos[2], st->universe[i][j]);
                fflush(NULL);
            }
        }
    }
    free(pos);
}