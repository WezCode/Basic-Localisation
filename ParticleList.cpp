
#include "ParticleList.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#define PARTICLE_SIZE 10000

#ifndef DEBUG
#define DEBUG 0
#endif


// Create a new empty list
ParticleList::ParticleList()
{
   //Inital Particles in the list - Scan through map, check for dots, create particle at that coordinate, store in list.
   //Need to initalise particles (the array) default values hold 0.
   particles[1000] = {};
   this->numParticles = 0;
}

//Copy constructor
ParticleList::ParticleList(ParticleList &otherList)
{
   numParticles = otherList.getNumberParticles();

   //Create new Particle for each particle in the otherList and then add it to the particles array in this instance of ParticleList
   for (int i = 0; i < otherList.getNumberParticles(); i++)
   {
      ParticlePtr newParticle = new Particle(otherList.get(i)->getX(), otherList.get(i)->getY(), otherList.get(i)->getOrientation());
      delete particles[i];
      particles[i] = nullptr;
      particles[i] = newParticle;
   }

   otherList.clear();
}

// Clean-up the particle list
ParticleList::~ParticleList()
{

}

// Number of particles in the ParticleList
int ParticleList::getNumberParticles()
{
   return numParticles;
}

// Get a pointer to the i-th particle in the list
ParticlePtr ParticleList::get(int i)
{
   return particles[i];
}

// Add a particle (as a pointer) to the list
//    This class now has control over the pointer
//    And should delete the pointer if the particle is removed from the list
void ParticleList::add_back(ParticlePtr particle)
{
   particles[numParticles] = particle;
   numParticles++;
}

// Remove all particlePtrs from the list, delete all particles
void ParticleList::clear()
{
   for (int i = 0; i < numParticles; i++)
   {
      delete particles[i];
      particles[i] = nullptr;
   }
   numParticles = 0;
}