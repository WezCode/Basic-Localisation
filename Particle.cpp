
#include "Particle.h"


// x-co-ordinate of the particle
int Particle::getX() {
   return x;
}

// y-co-ordinate of the particle
int Particle::getY() {
   return y;
}

// Orientation of the particle
Orientation Particle::getOrientation() {
   return o;
}

Particle::Particle(int tempX, int tempY, Orientation tempO) {
   x = tempX;
   y = tempY;
   o = tempO;
}

