#include <types.h>


void initParticle(particle* p, int* pos, int id){
    p->position.x = pos[0];
    p->position.y = pos[1];
    p->position.z = pos[2];

    p->id = id;
}

void printParticle(particle* p){
    printf("Particle %d: %d, %d, %d\n", p->id, p->position.x, 
                                     p->position.y, p->position.z);
}

//check if two particles have the same location:
int checkLocations(particle* p1, particle* p2){
    return p1->position.x == p2->position.x &&
            p1->position.y == p2->position.y &&
            p1->position.z == p2->position.z;
}

int checkParticleCollision(state* st, particle* p){
    for(int i = 0; i < st->collidedParticles; ++i){

        if(checkLocations(p, &(st->ctab[i]))){
            return 1;
        }
    }
    return 0;
}

void addCollidedParticle(state* st, particle* p){
    printf("PID: %d\n", p->id);
    ++st->collidedParticles;
    // particle* tmp;
    st->ctab = realloc(st->ctab, st->collidedParticles * sizeof(particle));
    // free(st->ctab);
    // st->ctab = tmp;
    // tmp = NULL;
    (st->ctab[st->collidedParticles - 1]).position.x = p->position.x;
    (st->ctab[st->collidedParticles - 1]).position.y = p->position.y;
    (st->ctab[st->collidedParticles - 1]).position.z = p->position.z;
    (st->ctab[st->collidedParticles - 1]).id = p->id;
    printf("PARTICLE INFORMATION:\n");
    printParticle(&(st->ctab[st->collidedParticles - 1]));

    //Scan the list of bodies (up until the end), move the particle we want to remove to the end:
    if(st->activeParticles > 0){
        for(int i = 0; i < st->activeParticles - 2; ++i){
            if(&(st->ptab[i]) == p){
                st->ptab[i] = st->ptab[st->activeParticles - 1];
            }
        }
        --st->activeParticles;
        if(st->activeParticles > 0){
            st->ptab = realloc(st->ptab, st->activeParticles * sizeof(particle));
            // free(st->ptab);
            // st->ptab = tmp;
            // tmp = NULL;
        }
    }
}

void capParticle(context* ctx, particle* p){
    if(p->position.x > ctx->max[0]){
        p->position.x = ctx->max[0];
    }
    else if(p->position.x < 0){
        p->position.x = 0;
    }

    if(p->position.y > ctx->max[1]){
        p->position.y = ctx->max[1];
    }
    else if(p->position.y < 0){
        p->position.y = 0;
    }

    if(p->position.z > ctx->max[2]){
        p->position.z = ctx->max[2];
    }
    else if(p->position.z < 0){
        p->position.z = 0;
    }
}

void updateParticlePosition(state* st, context* ctx, particle* p){
    int num = rand() % 6;
    int dx, dy, dz;
    dx = dy = dz = 0;
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
    p->position.x += dx;
    p->position.y += dy;
    p->position.z += dz;
    if(!checkParticleCollision(st, p)){
    //Check for collision here
        capParticle(ctx, p);
    }
    else{
        p->position.x -= dx;
        p->position.y -= dy;
        p->position.z -= dz;
        addCollidedParticle(st, p);
    }
}


void initContext(context** ctx, int* data){
    *ctx = calloc(1, sizeof(context));
    (*ctx)->max = calloc(3, sizeof(int));
    (*ctx)->numParticles = data[0];
    (*ctx)->numSteps = data[1];
    (*ctx)->max[0] = 10;
    (*ctx)->max[1] = 10;
    (*ctx)->max[2] = 10;
}

void initState(state** st, context* ctx){
    *st = calloc(1, sizeof(state));
    int* pos = (int*)calloc(3, sizeof(int));
    
    //Random Variable Initialization
    time_t t;
    srand((unsigned) time(&t));

    (*st)->ptab = calloc(ctx->numParticles, sizeof(particle));
    for(int i = 0; i < ctx->numParticles; ++i){
        pos[0] = rand() % ctx->max[0];
        pos[1] = rand() % ctx->max[1];
        pos[2] = rand() % ctx->max[2];
        initParticle(&(((*st)->ptab)[i]), pos, i);
    }

    (*st)->activeParticles = ctx->numParticles;
    (*st)->collidedParticles = 0;
    (*st)->simSteps = 0;
}

void initAggregators(state* st, char* agFile){
    printf("READING AG DATA FROM FILE: %s\n", agFile);
    
    int* pos = (int*)calloc(3, sizeof(int));
    char* buff = (char*)calloc(8, sizeof(char));
    char* lineBuffer = (char*)calloc(32, sizeof(char));
    FILE* fp;
    fp = fopen(agFile, "r");
    while(fscanf(fp, "%d %d %d", &pos[0], &pos[1], &pos[2]) != EOF){
        st->ctab = realloc(st->ctab, (st->collidedParticles + 1) * sizeof(particle));
        initParticle(&(st->ctab[st->collidedParticles]), pos, (st->collidedParticles + 1) * -1);
        ++st->collidedParticles;
    }
    free(pos);
    free(buff);
    free(lineBuffer);
}


void printState(state* st, context* ctx){
    printf("ACTIVE PARTICLES:\n =========================\n");
    for(int i = 0; i < st->activeParticles; ++i){
        printParticle(&(st->ptab[i]));
    }
    printf("COLLIDED PARTICLES:\n =========================\n");
    for(int i = 0; i < st->collidedParticles; ++i){
        printParticle(&(st->ctab[i]));
    }
}