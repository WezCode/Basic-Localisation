
#include "Particle.h"
#include "ParticleFilter.h"
#include "Types.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <tuple>
#include <string>
#include <vector>

/*
 * Running a unit test requires three text-files:
 *    1. The maze
 *    2. The sequence of robot observations
 *    3. The final positions of the robot (un-sorted)
 *
 * Full command
 *    ./unit_tests <testname>
 *
 * For example:
 *   ./unit_tests sampleTest/sample01
 *
 */

#define ARGV_TEST 1
#define EXT_MAZE std::string(".maze")
#define EXT_OBS std::string(".obs")
#define EXT_POS std::string(".pos")
#define REQ_ARGS 1
#define TUPLE_X 0
#define TUPLE_Y 1
#define TUPLE_ORIEN 2

#define DEBUG 1

// Useful typedef for positions
typedef std::tuple<int, int, Orientation> Position;

// Data contens
class Data
{
 public:
   Data() : maze(NULL),
            rows(0),
            cols(0),
            observations(){};

   Grid maze;
   int rows;
   int cols;
   std::vector<Grid> observations;
   std::vector<Position> positions;
};
typedef Data *DataPtr;

// Helper methods for making and deleting grids
Grid make_grid(const int rows, const int cols);
void delete_grid(Grid grid, int rows, int cols);

// Functions
void check_args(int argc, char **argv);
void load_data(char **argv, DataPtr data);
void load_data_maze(char **argv, DataPtr data);
void load_data_obs(char **argv, DataPtr data);
void load_data_pos(char **argv, DataPtr data);
void load_lines(std::ifstream &in, std::vector<std::string> &lines);
bool match_positions(Position &posTest, ParticlePtr posPf);
bool run_unit_test(DataPtr data);

int main(int argc, char **argv)
{

   try
   {
      // Check args
      check_args(argc, argv);

      // Load file contents into data structure
      DataPtr data(new Data());
      load_data(argv, data);

      // Run actual test
      if (DEBUG)
      {
         std::cout << "Running Unit Test" << std::endl;
      }
      bool testPassed = run_unit_test(data);
      if (testPassed)
      {
         std::cout << "Test Passed" << std::endl;
      }
      else
      {
         std::cout << "Test Failed" << std::endl;
      }
   }
   catch (std::runtime_error &exception)
   {
      std::cout << "ERROR: Caught Error: "
                << exception.what()
                << std::endl;
   }

   return 0;
}

void check_args(int argc, char **argv)
{
   if (argc != REQ_ARGS + 1)
   {
      throw std::runtime_error("Not enough file names provided");
   }

   std::string mazeFilename = argv[ARGV_TEST] + EXT_MAZE;
   std::string obsFilename = argv[ARGV_TEST] + EXT_OBS;
   std::string posFilename = argv[ARGV_TEST] + EXT_POS;

   if (DEBUG)
   {
      std::cout << "Maze filename: " << mazeFilename << std::endl;
      std::cout << "Observations filename: " << obsFilename << std::endl;
      std::cout << "Positions filename: " << posFilename << std::endl;
   }

   // Check files exist
   auto checkFile = [](std::string &filename) {
      std::ifstream in;
      in.open(filename);
      if (in.fail())
      {
         throw std::runtime_error("Could not open file '" + filename + "'");
      }
      in.close();
   };

   checkFile(mazeFilename);
   checkFile(obsFilename);
   checkFile(posFilename);
}

void load_data(char **argv, DataPtr data)
{
   load_data_maze(argv, data);
   load_data_obs(argv, data);
   load_data_pos(argv, data);
}

void load_data_maze(char **argv, DataPtr data)
{
   std::string filename = argv[ARGV_TEST] + EXT_MAZE;

   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);
   if (lines.size() == 0)
   {
      throw std::runtime_error("No Maze in file");
   }

   // Create data
   int width = lines.front().size();
   int height = lines.size();
   data->maze = make_grid(height, width);

   // Load Maze
   for (unsigned int row = 0; row != lines.size(); ++row)
   {
      std::string &line = lines[row];

      // Check width
      if (line.size() != (unsigned int)width)
      {
         throw std::runtime_error("Maze dimensions not consistent");
      }

      // Load data
      for (unsigned int col = 0; col != line.size(); ++col)
      {
         data->maze[row][col] = (char)line[col];
      }
   }
   data->rows = height;
   data->cols = width;
   if (DEBUG)
   {
      std::cout << "Loaded Maze: " << std::endl;
      for (int row = 0; row != data->rows; ++row)
      {
         for (int col = 0; col != data->cols; ++col)
         {
            std::cout << data->maze[row][col];
         }
         std::cout << std::endl;
      }
   }
}

void load_data_obs(char **argv, DataPtr data)
{
   std::string filename = argv[ARGV_TEST] + EXT_OBS;

   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);

   // Remove all blank lines
   for (auto it = lines.begin(); it != lines.end();)
   {
      if (it->size() == 0)
      {
         it = lines.erase(it);
      }
      else
      {
         ++it;
      }
   }

   if (lines.size() == 0)
   {
      throw std::runtime_error("No observations in file");
   }
   if (lines.size() % 3 != 0)
   {
      throw std::runtime_error("Observations not set of 3x3 grids");
   }

   // Load each observation
   int row = 0;
   for (std::string &line : lines)
   {
      // Check width
      if (line.size() != OBSERVATION_DIMENSION)
      {
         throw std::runtime_error("Observation dimensions not 3x3");
      }

      // Create data if necessary
      if (row == 0)
      {
         char **obs = make_grid(OBSERVATION_DIMENSION, OBSERVATION_DIMENSION);
         data->observations.push_back(obs);
      }

      // Load data
      char **obs = data->observations.back();
      for (unsigned int col = 0; col != line.size(); ++col)
      {
         obs[row][col] = line[col];
      }

      // Increment
      ++row;
      if (row == OBSERVATION_DIMENSION)
      {
         row = 0;
      }
   }
   if (DEBUG)
   {
      std::cout << "Loaded "
                << data->observations.size()
                << " observations" << std::endl;
   }
}

void load_data_pos(char **argv, DataPtr data)
{
   std::string filename = argv[ARGV_TEST] + EXT_POS;

   std::ifstream in(filename);
   std::vector<std::string> lines;
   load_lines(in, lines);
   if (DEBUG)
   {
      std::cout << "Got: " << lines.size() << " positions" << std::endl;
   }

   // Process each position
   for (std::string &line : lines)
   {
      std::regex regex("^[(]([0-9]+),([0-9]+),([a-zA-Z]+)[)]$");
      std::smatch match;
      bool ok = std::regex_match(line, match, regex);

      if (ok)
      {
         Position position;
         std::get<TUPLE_X>(position) = std::stoi(match[1].str());
         std::get<TUPLE_Y>(position) = std::stoi(match[2].str());
         std::string orientation = match[3].str();
         std::transform(orientation.begin(), orientation.end(), orientation.begin(), ::tolower);
         if (orientation == ORIEN_STR_LEFT)
         {
            std::get<TUPLE_ORIEN>(position) = ORIEN_LEFT;
         }
         else if (orientation == ORIEN_STR_UP)
         {
            std::get<TUPLE_ORIEN>(position) = ORIEN_UP;
         }
         else if (orientation == ORIEN_STR_RIGHT)
         {
            std::get<TUPLE_ORIEN>(position) = ORIEN_RIGHT;
         }
         else if (orientation == ORIEN_STR_DOWN)
         {
            std::get<TUPLE_ORIEN>(position) = ORIEN_DOWN;
         }

         data->positions.push_back(position);
      }
      else
      {
         throw std::runtime_error("Position format incorrect");
      }
   }
}

void load_lines(std::ifstream &in, std::vector<std::string> &lines)
{
   while (!in.eof())
   {
      std::string line;
      in >> line;

      // Drop newline
      if (line.back() == '\n')
      {
         line.pop_back();
      }

      // Add
      lines.push_back(line);
   }
   if (lines.back().size() == 0)
   {
      lines.pop_back();
   }
}

bool match_positions(Position &posTest, ParticlePtr posPf)
{
   bool match = false;

   match = std::get<TUPLE_X>(posTest) == posPf->getX() && std::get<TUPLE_Y>(posTest) == posPf->getY() && std::get<TUPLE_ORIEN>(posTest) == posPf->getOrientation();

   return match;
}

bool run_unit_test(DataPtr data)
{
   bool testPassed = false;

   // Create ParticleFilter
   if (DEBUG)
   {
      std::cout << "Create particle filter" << std::endl;
   }
   ParticleFilter *pf = new ParticleFilter(data->maze, data->rows, data->cols);

   // Iterate through observations
   int obsCount = 1;
   for (Grid &obs : data->observations)
   {
      if (DEBUG)
      {
         std::cout << "Giving Observation: " << obsCount << std::endl;
      }
      pf->newObservation(obs);
      ++obsCount;
   }

   // Get final positions
   ParticleList *finalPositions = pf->getParticles();
   int numPositions = finalPositions->getNumberParticles();
   if (numPositions > 0)
   {
      // ParticlePtr finalPositions[numPositions];
      // pf->getParticles(finalPositions);
      if (DEBUG)
      {
         std::cout << "Final Positions:" << std::endl;
         for (int i = 0; i != numPositions; ++i)
         {
            std::cout << "("
                      << finalPositions->get(i)->getX()
                      << ","
                      << finalPositions->get(i)->getY()
                      << ","
                      << finalPositions->get(i)->getOrientation()
                      << ")"
                      << std::endl;
         }
      }

      // Test all particles are expected
      if ((unsigned int)numPositions == data->positions.size())
      {
         std::map<int, bool> checked;
         for (Position &posTest : data->positions)
         {
            for (int i = 0; i != numPositions; ++i)
            {
               if (match_positions(posTest, finalPositions->get(i)))
               {
                  checked[i] = true;
               }
            }
         }

         // If not enough true items in the map, test failed
         testPassed = checked.size() == data->positions.size();
      }
   }
   else
   {
      if (DEBUG)
      {
         std::cout << "Particle Filter returned zero particles" << std::endl;
      }
   }

   // Delete Particle list
   delete finalPositions;

   // Delete Particle Filter
   delete pf;

   return testPassed;
}

Grid make_grid(const int rows, const int cols)
{
   Grid grid = NULL;

   if (rows >= 0 && cols >= 0)
   {
      grid = new char *[rows];
      for (int i = 0; i != rows; ++i)
      {
         grid[i] = new char[cols];
      }
   }

   return grid;
}

void delete_grid(Grid grid, int rows, int cols)
{
   if (rows >= 0 && cols >= 0)
   {
      for (int i = 0; i != rows; ++i)
      {
         delete grid[i];
      }
      delete grid;
   }

   return;
}
