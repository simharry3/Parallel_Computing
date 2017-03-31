#include "particle.h"
#include <stdio.h>
#include <stdlib.h>

Particle::Particle(){
    position = (int*)calloc(3, sizeof(int));
    setPosition(0,0,0);
}

void Particle::setPosition(int x, int y , int z){
    position[0] = x;
    position[1] = y;
    position[2] = z;
}