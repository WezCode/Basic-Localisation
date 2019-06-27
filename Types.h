
#ifndef COSC_ASS_ONE_TYPES
#define COSC_ASS_ONE_TYPES

// Orientation codes
#define ORIEN_LEFT      0
#define ORIEN_UP        1
#define ORIEN_RIGHT     2
#define ORIEN_DOWN      3

#define ORIEN_STR_LEFT     "left"
#define ORIEN_STR_UP       "up"
#define ORIEN_STR_RIGHT    "right"
#define ORIEN_STR_DOWN     "down"

// Define Oriendtation Type
typedef int Orientation;

// A 2D array to represent the maze or observations
// REMEMBER: in a grid, the location (x,y) is found by grid[y][x]!
typedef char** Grid;

// Observation information
#define OBSERVATION_DIMENSION    3

#endif // COSC_ASS_ONE_TYPES
