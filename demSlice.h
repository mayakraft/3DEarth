#ifndef GISOSX_DEM_h
#define GISOSX_DEM_h


// all latitude and longitude inputs are for the center of the square
// height and width equally spaced around it


// 2D square plot, size of width * height
int16_t* elevationForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height);

// square plot of land point cloud
//   array size: width * height * 3 (x,y,z)
//   X:longitude Y:latitude Z:elevation
float* elevationPointsForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height);

// convert latitude longitude to a byte location in DEM file
unsigned long getByteOffset(float latitude, float longitude);

#endif
