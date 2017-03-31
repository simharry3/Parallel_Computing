#ifndef _PARTICLE_H_INCLUDED_
#define _PARTICLE_H_INCLUDED_

using namespace std;

class Particle{
public:
    Particle();
    int* getPosition();
    void setPosition(int, int, int);
private:
    int* position;
};

#endif