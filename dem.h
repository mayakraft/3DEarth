#ifndef GISOSX_DEM_h
#define GISOSX_DEM_h

#include <string>

using std::string;
typedef struct demMeta demMeta;

// all latitude and longitude inputs are for the center of the square
// height and width equally spaced around it

// pass directory as usual, but declare file WITHOUT an extension
// function will seek out .DEM file (data) and separate files for metadata
float *elevationPoints(string directory, string file, double latitude, double longitude, unsigned int width, unsigned int height);

demMeta loadHeader(string directory, string filename);

// returns raw DEM data in a smaller cropped rectangle
// includes edge overflow protection
int16_t* cropDEM(string directory, string filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// 2D square plot, size of width * height
int16_t* elevationForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height);

// square plot of land point cloud
//   array size: width * height * 3 (x,y,z)
//   X:longitude Y:latitude Z:elevation
float* elevationPointsForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height);

float* elevationPointsFromDEM(string directory, string filename, float latitude, float longitude, unsigned int width, unsigned int height);

// convert latitude longitude to a byte location in DEM file
//unsigned long getByteOffset(float latitude, float longitude);

#endif
