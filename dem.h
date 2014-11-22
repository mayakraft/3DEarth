#ifndef GISOSX_DEM_h
#define GISOSX_DEM_h

#include <string.h>

// typedef struct demMeta demMeta;

// all latitude and longitude inputs are for the center of the square
// height and width equally spaced around it

// pass directory as usual, but declare file WITHOUT an extension
// function will seek out .DEM file (data) and separate files for metadata
float *elevationPoints(char *directory, char *file, double latitude, double longitude, unsigned int width, unsigned int height);

struct demMeta loadHeader(char *directory, char *filename);

// returns raw DEM data in a smaller cropped rectangle
// includes edge overflow protection
int16_t* cropDEM(char *directory, char *filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// point cloud rectangular plot of land
// OpenGL data:
//  - array size: width * height * 3 (x,y,z)
//  - X:longitude Y:latitude Z:elevation
void elevationPointCloud(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float** points, float** colors);

// convert latitude longitude to a byte location in DEM file
//unsigned long getByteOffset(float latitude, float longitude);

#endif
