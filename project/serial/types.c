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

int checkParticleCollision(state* st, particle* p, int dx, int dy, int dz){
    for(int i = 0; i < st->collidedParticles; ++i){
        if(checkLocations(p, &(st->ctab[i]))){
            return 1;
        }
    }
    return 0;
}

void addCollidedParticle(state* st, particle* p){

}

void updateParticlePosition(state* st, particle* p){
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
    if(!checkParticleCollision(st, p, dx, dy, dz)){
    //Check for collision here
        p->position.x += dx;
        p->position.y += dy;
        p->position.z += dz;
    }
    else{
        addCollidedParticle(st, p);
    }
}


void initContext(context** ctx, int* data){
    *ctx = calloc(1, sizeof(context));
    (*ctx)->max = calloc(3, sizeof(int));
    (*ctx)->numParticles = data[0];
    (*ctx)->numSteps = data[1];
    (*ctx)->max[0] = 100;
    (*ctx)->max[1] = 100;
    (*ctx)->max[2] = 100;
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