/*
   CODE DESCRIPTION
   --------------------------------------------------------------------------------------------------

   APPROACH: I took the approach of having two separate methods, one for knowing the orientation and another for not knowing the orientation.
   In addition to this, for both cases I had methods for the first observation and generating the initial particles (initialParticlesKnownOrient, 
   initialParticleUnknownOrient). These were followed by their secondary methods (checkKnownOrientation, checkUnknownOrientation).

   MAIN ISSUES: The main issues I encountered were:
   - Managing memory:
     Understanding the deletion of pointers, and deallocating memory on the heap. Conceptually understanding object ownership, e.g. ParticleList "owns"
     the particles it creates thus is responsible for deleting particles etc. 

   CHOICES:
   - With the goal of having low coupling and high cohesion (in respect to methods), I split my code into many smaller methods
   that were focused on a small aspect of a larger method. This also improved the readability of methods.
   - Wherever possible, memory was allocated to the stack instead of the heap. This also meant that the destructors were automatically
   called as soon as the object/variable left the scope, hence removing the need to implement the destructor in particleFilter.
   - I prioritised code readability over concise and complex code to allow better abstraction. E.g. For methods checkUp, checkLeft,
   checkRight, checkDown etc, they're function is quite similar but they are there own methods so it makes sense when calling it
   in checkUnknownOrientation. Naturally, in respect to comments, methods that were similar in function were not documented again with the same comments. 
   - The algorithm on the specification stated to "remove" particles from P' (tempList in my case), but instead of removing them and then having to
   make pList = tempList. I simply added the particles that matched directly to pList.


   EFFICIENCY/QUALITY: 
   - Throughout my code, I attempted to achieve efficiency in terms of speed and memory. 
       1. When checking individual particles against observations, as soon as a mismatch occured, it would begin checking the next particle etc.
       2. Objects and variables would be allocated to the stack as opposed to the heap. 
         -In this way, memory was allocated during the period of time that the object/variable is in use, and it's destructor would be called as
          soon as its out of scope. To my understanding, accessing memory on the stack is also faster. 
       3. I intentionally created two separate algorithms for handling observations with known and unknown orientations. Instead of creating
       one larger general method to potentially solve both cases, these specific methods we're created to make assumptions where necessary
       to speed up the process of determining possible particles. 
         -For example, in knowing the orientation, particles are automatically created to have orientations equal to the observations, and simple 
         tests to see if the orientation changed between observations allowed for quick deductions as to whether or not it rotated or moved etc.
   - In saying this, there are also places where my code could be more efficient.
      1. If a dynamic array was used, memory would only have to be allocated equal to the amount of particles needed to be stored each time.


*/

#include "ParticleFilter.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#define CENTER_OF_OBS currentObservation[1][1]
#define CENTER_OF_PREV_OBS previousObservation[1][1]

Grid currentObservation;
ParticleList pList;

// Initialise a new particle filter with a given maze of size (x,y)
ParticleFilter::ParticleFilter(char **mazeValue, int rowsValue, int colsValue)
{
   maze = mazeValue;
   rows = rowsValue;
   cols = colsValue;
}

// Clean-up the Particle Filter
ParticleFilter::~ParticleFilter()
{
}

// A new observation of the robot, of size 3x3
void ParticleFilter::newObservation(Grid observation)
{
   previousObservation = currentObservation;
   currentObservation = observation;
   updateParticleList();
}

// Return a DEEP COPY of the ParticleList of all particles representing
//    the current possible locations of the robot
ParticleList *ParticleFilter::getParticles()
{
   ParticleList *pList2 = new ParticleList(pList);
   return pList2;
}

//Compare each particle in the ParticleList (pList) to the current observation.
//Determines the correct method to update pList, e.g. whether its a first observation, unknown orientation etc.
void ParticleFilter::updateParticleList()
{
   if (previousObservation == NULL)
   {
      if (CENTER_OF_OBS != '*')
      {
         initialParticlesKnownOrient();
      }
      else
      {
         initialParticlesUnknownOrient();
      }
   }
   else
   {
      if (CENTER_OF_OBS != '*')
      {
         checkKnownOrientation();
      }
      else
      {
         checkUnknownOrientation();
      }
   }
}

void ParticleFilter::initialParticlesKnownOrient()
{
   //Creates particles where the orientation is equal to the orientation of the current observation.
   for (int y = 0; y < rows; y++)
   {
      for (int x = 0; x < cols; x++)
      {
         if (maze[y][x] == '.')
         {
            Particle p(x, y, getOrientationOfObs());
            if (checkParticles(p))
            {
               pList.add_back(new Particle(p.getX(), p.getY(), p.getOrientation()));
            }
         }
      }
   }
}

//Checks all particles against all new observations after first observation.
void ParticleFilter::checkKnownOrientation()
{
   ParticleList tempList;
   //If the middle square AKA orientation is not the same between chronological observations, a rotation occured.
   if (CENTER_OF_PREV_OBS != CENTER_OF_OBS)
   {
      //Update the orientation of all particles in pList.
      for (int i = 0; i < pList.getNumberParticles(); i++)
      {
         //Create a new particle for each particle with updated orientation, add to tempList.
         ParticlePtr particle = new Particle(pList.get(i)->getX(), pList.get(i)->getY(), getOrientationOfObs());
         tempList.add_back(particle);
      }
      pList.clear();
      pList = tempList;
   }
   else
   {
      //Create a particle for each particle in pList with the shift added
      for (int i = 0; i < pList.getNumberParticles(); i++)
      {
         Particle particle(shiftParticle(pList.get(i)));
         //Check each shifted particle
         if (checkParticles(particle))
         {
            //Add particle if it matches
            tempList.add_back(new Particle(particle.getX(), particle.getY(), particle.getOrientation()));
         }
      }
      pList.clear();
      pList = tempList;
   }
}

//MILESTONE 3 METHODS

//Creates a particle and checks if rotations of that particle match the observation.
//If any do, add to pList.
void ParticleFilter::initialParticlesUnknownOrient()
{
   for (int y = 0; y < rows; y++)
   {
      for (int x = 0; x < cols; x++)
      {
         if (maze[y][x] == '.')
         {
            Particle p(x, y, 1);
            if (checkUp(p))
            {
               pList.add_back(new Particle(p.getX(), p.getY(), orientOfParticle));
            }
            if (checkRight(p))
            {
               pList.add_back(new Particle(p.getX(), p.getY(), orientOfParticle));
            }
            if (checkLeft(p))
            {
               pList.add_back(new Particle(p.getX(), p.getY(), orientOfParticle));
            }
            if (checkDown(p))
            {
               pList.add_back(new Particle(p.getX(), p.getY(), orientOfParticle));
            }
         }
      }
   }
}

//If orientation is unknown, it could have rotated left, right or shifted one move.
//Check each of these options against current observation and add to pList accordingly.
void ParticleFilter::checkUnknownOrientation()
{
   ParticleList tempList;
   for (int i = 0; i < pList.getNumberParticles(); i++)
   {
      Particle p = *(pList.get(i));
      if (p.getOrientation() == 0 || p.getOrientation() == 2)
      {
         if (checkUp(p) || checkDown(p))
         {
            tempList.add_back(new Particle(p.getX(), p.getY(), orientOfParticle));
         }
      }
      else if (p.getOrientation() == 1 || p.getOrientation() == 3)
      {
         if (checkLeft(p) || checkRight(p))
         {
            tempList.add_back(new Particle(p.getX(), p.getY(), orientOfParticle));
         }
      }
      p = shiftParticle(pList.get(i));
      if (checkUp(p) || checkLeft(p) || checkRight(p) || checkDown(p))
      {
         tempList.add_back(new Particle(p.getX(), p.getY(), p.getOrientation()));
      }
   }
   pList.clear();
   pList = tempList;
}

//The general method use to check particles against observations
bool ParticleFilter::checkParticles(Particle p)
{
   bool match = true;
   for (int j = -1; j < 2; j++)
   {
      for (int k = -1; k < 2; k++)
      {
         if (maze[p.getY() + j][p.getX() + k] != currentObservation[j + 1][k + 1])
         {
            //If one square doesn't match, shouldn't keep checking following squares
            int tempj = j;
            int tempk = k;
            //The particle does not match the observation, so don't add to list
            match = false;
            //Don't check for next coordinate by setting j and k to end loops. (e.g start checking next particle)
            j = 2;
            k = 2;
            //Since the middle of the particle is always a dot instead of a <v>^ (robot), a mismatch will always occur when checking the middle square
            //An exemption is made for this case
            if (tempj == 0 && tempk == 0)
            {
               //Don't stop checking the following squares
               j = tempj;
               k = tempk;
               match = true;
            }
         }
      }
   }
   return match;
}

bool ParticleFilter::checkUp(Particle particle)
{
   orientOfParticle = particle.getOrientation();
   return checkParticles(particle);
}

bool ParticleFilter::checkRight(Particle particle)
{
   bool match = true;
   for (int k = 1; k > -2; k--)
   {
      for (int j = -1; j < 2; j++)
      {
         if (maze[particle.getY() + j][particle.getX() + k] != currentObservation[-k + 1][j + 1])
         {
            int tempj = j;
            int tempk = k;
            match = false;
            j = 2;
            k = -2;
            if (tempj == 0 && tempk == 0)
            {
               j = tempj;
               k = tempk;
               match = true;
            }
         }
      }
   }
   if (match)
   {
      orientOfParticle = particle.getOrientation() + 1;
      if (orientOfParticle > 3)
      {
         orientOfParticle -= 3;
      }
   }
   return match;
}

bool ParticleFilter::checkDown(Particle particle)
{
   bool match = true;
   for (int k = 1; k > -2; k--)
   {
      for (int j = 1; j > -2; j--)
      {
         if (maze[particle.getY() + j][particle.getX() + k] != currentObservation[-j + 1][-k + 1])
         {
            int tempj = j;
            int tempk = k;
            match = false;
            j = -2;
            k = -2;
            if (tempj == 0 && tempk == 0)
            {
               j = tempj;
               k = tempk;
               match = true;
            }
         }
      }
   }
   if (match)
   {
      orientOfParticle = particle.getOrientation() + 2;
      if (orientOfParticle > 3)
      {
         orientOfParticle -= 3;
      }
   }
   return match;
}

bool ParticleFilter::checkLeft(Particle particle)
{
   bool match = true;
   for (int k = -1; k < 2; k++)
   {
      for (int j = 1; j > -2; j--)
      {
         if (maze[particle.getY() + j][particle.getX() + k] != currentObservation[k + 1][-j + 1])
         {
            int tempj = j;
            int tempk = k;
            match = false;
            j = -2;
            k = 2;
            if (tempj == 0 && tempk == 0)
            {
               j = tempj;
               k = tempk;
               match = true;
            }
         }
      }
   }
   if (match)
   {
      orientOfParticle = particle.getOrientation() - 1;
      if (orientOfParticle < 0)
      {
         orientOfParticle = 3;
      }
   }
   return match;
}

Orientation ParticleFilter::getOrientationOfObs()
{
   Orientation o;
   if (CENTER_OF_OBS == '<')
   {
      o = ORIEN_LEFT;
   }
   else if (CENTER_OF_OBS == '^')
   {
      o = ORIEN_UP;
   }
   else if (CENTER_OF_OBS == '>')
   {
      o = ORIEN_RIGHT;
   }
   else
   {
      o = ORIEN_DOWN;
   }
   return o;
}

//Returns shifted particle
Particle ParticleFilter::shiftParticle(ParticlePtr particle)
{
   int horizontalShift;
   int verticalShift;

   if (particle->getOrientation() == ORIEN_LEFT)
   {
      horizontalShift = -1;
      verticalShift = 0;
   }
   else if (particle->getOrientation() == ORIEN_UP)
   {
      horizontalShift = 0;
      verticalShift = -1;
   }
   else if (particle->getOrientation() == ORIEN_RIGHT)
   {
      horizontalShift = 1;
      verticalShift = 0;
   }
   else
   {
      horizontalShift = 0;
      verticalShift = 1;
   }
   Particle newParticle(particle->getX() + horizontalShift, particle->getY() + verticalShift, particle->getOrientation());
   return newParticle;
}
