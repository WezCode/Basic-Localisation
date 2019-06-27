
#ifndef COSC_ASS_ONE_PARTICLE_LIST
#define COSC_ASS_ONE_PARTICLE_LIST

#include "Particle.h"
#include "Types.h"

class ParticleList {
public:

   /*                                           */
   /* DO NOT MOFIFY ANY CODE IN THIS SECTION    */
   /*                                           */


   // Create a New Empty List
   ParticleList();

   // Clean-up the particle list
   ~ParticleList();

   // Number of particles in the ParticleList
   int getNumberParticles();

   // Get a pointer to the i-th particle in the list
   ParticlePtr get(int i);

   // Add a particle (as a pointer) to the list
   //    This class now has control over the pointer
   //    And should delete the pointer if the particle is removed from the list
   void add_back(ParticlePtr particle);

   // Remove all particles from the list
   // Don't forget to clean-up the memory!
   void clear();

   /*                                           */
   /* YOU MAY ADD YOUR MODIFICATIONS HERE       */
   /*                                           */

   //Copy constructor for ParticleList
   ParticleList(ParticleList &otherList);

private:
   ParticlePtr    particles[1000];
   int            numParticles;


};

#endif // COSC_ASS_ONE_PARTICLE_LIST
