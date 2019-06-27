
#ifndef COSC_ASS_ONE_PARTICLE_FILTER
#define COSC_ASS_ONE_PARTICLE_FILTER

#include "Particle.h"
#include "ParticleList.h"
#include "Types.h"

#define MAZE_SIZE 100

class ParticleFilter
{
public:
  /*                                           */
  /* DO NOT MOFIFY ANY CODE IN THIS SECTION    */
  /*                                           */

  // Initialise a new particle filter with a given maze of size (x,y)
  ParticleFilter(Grid maze, int rows, int cols);

  // Clean-up the Particle Filter
  ~ParticleFilter();

  // A new observation of the robot, of size 3x3
  void newObservation(Grid observation);

  // Return a DEEP COPY of the ParticleList of all particles representing
  //    the current possible locations of the robot
  ParticleList *getParticles();

  //Own impementation

  //Populate the particle list with initial particles (all the dots)
  void initialParticlesKnownOrient();
  void initialParticlesUnknownOrient();

  void updateParticleList();

  void checkKnownOrientation();
  void checkUnknownOrientation();

  bool checkUp(Particle particle);
  bool checkRight(Particle particle);
  bool checkDown(Particle particle);
  bool checkLeft(Particle particle);

  Orientation getOrientationOfObs();
  Particle shiftParticle(ParticlePtr particle);
  bool checkParticles(Particle p);

private:
  Grid maze;
  Grid previousObservation;
  Orientation orientOfParticle;
  int rows;
  int cols;
};

#endif // COSC_ASS_ONE_PARTICLE_FILTER
