#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "operations.h"

int main(int argc, char* argv[]){
    printf("HERE WE GO\n");
    state* st;
    context* ctx;

    //<NUM PARTICLES> <NUM STEPS>
    int* data = calloc(argc - 1, sizeof(int));
    for(int i = 1; i < argc - 1; ++i){
        data[i - 1] = strtol(argv[i], NULL, 10);
    }
    initContext(&ctx, data);
    initState(&st, ctx);
    initAggregators(st, argv[argc-1]);
    printState(st, ctx);
    runSystem(st, ctx);
    printState(st, ctx);
    printSimulationResults(st, ctx);
    exit(0);
}