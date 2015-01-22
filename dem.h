#ifndef GISOSX_DEM_h
#define GISOSX_DEM_h


// OPENGL MESH BUILDER
// --------------------------------------------------
// OPENGL DATA FORMAT
//    array size: width * height * 3 (x,y,z)
//    X:longitude Y:latitude Z:elevation
//
// SPECIFY CENTER OF TILES
//    to grab a location: specify center of the rectangle (not top left corner)
//    height and width of rectangle in degrees (Lat/Long)
//
// FILE SPECIFICATION
//    pass in the directory as usual, but specify file WITHOUT an extension
//    and the function will seek out .DEM file (data) as well as metadata files

void elevationPointCloud(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float** points, float** colors, unsigned int *numPoints);

void elevationTriangles(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float **points, uint32_t **indices, float **colors, unsigned int *numPoints, unsigned int *numIndices);

void elevationTriangleStrip(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float *points, float *colors);



// DEM PROCESSING
// --------------------------------------------------
//
// .DEM HEADER FILE SPEC INCLUDES:
// (unsigned int)	nrows:		number of data rows
// (unsigned int)	ncols:		number of data columns
// (double)			ulxmap:		top corner, X location (degrees longitude)
// (double)			ulymap:		top corner, Y location (degrees latitude)
// (double)			xdim:		degrees per step along X (degrees longitude)
// (double)			ydim:		degrees per step along Y (degrees latitude)

// loads data from .HDR file (packaged with .DEM files from USGS)
struct demMeta loadHeader(char *directory, char *filename);

// CROPPING DEM FILES
//   returns raw DEM data in a smaller cropped rectangle
//   includes edge overflow protection
//   rect defined by (x,y):top left corner and width, height
int16_t* cropDEM(char *directory, char *filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
//   same as above, if you already have the DEM header loaded into a demMeta struct
int16_t* cropDEMWithMeta(char *directory, char *filename, struct demMeta meta, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// LAT LONG -> BYTE CONVERSION
//   using location information found in header file,
//   returns index of precise byte for a latitude, longitude
//   IF ERROR (lat,long exceed tile boundary) sets col and row to -1 
void getByteColumnRowFromGeoLocation(struct demMeta meta, float latitude, float longitude, unsigned int *col, unsigned int *row);
//   returns index of precise byte for a latitude, longitude
//   IF ERROR returns -1
unsigned long getByteOffsetFromGeoLocation(struct demMeta meta, float latitude, float longitude);

// EDGE OVERFLOW PROTECTION
//   if exceeds boundary, returns closest valid rectangle 
//   which still fits inside .DEM boundaries 
void checkBoundaries(struct demMeta meta, unsigned int *x, unsigned int *y, unsigned int *width, unsigned int *height);

#endif
