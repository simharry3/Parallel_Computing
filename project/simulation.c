#include <stdio.h>
#include <stdlib.h>
#include "types.h"

int main(int argc, char* argv[]){
    printf("HERE WE GO\n");
    state* st;
    context* ctx;
    initContext(&ctx);
    initState(&st, ctx);

    printState(st, ctx);

    exit(0);
}