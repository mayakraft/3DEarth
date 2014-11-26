#elevation maps

OpenGL mesh rectangles of Earth surface from anywhere in the world using the USGS Global 30 Arc-Second Elevation (GTOPO30) data

download tiles for offline use: [ftp://edcftp.cr.usgs.gov/data/gtopo30]

![tiles image](https://lta.cr.usgs.gov/sites/default/files/tiles.gif)

#methods

```c
 // mallocs GL_POINTS (x,y,z), stored in "points" with size of width*height
void elevationPointCloud(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float** points, float** colors, unsigned int *numPoints);

void elevationTriangles(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float **points, uint32_t **indices, float **colors, unsigned int *numPoints, unsigned int *numIndices);
```

* lat/lon mark the center of the plate
* width and height are in km
* filename *without* extension: will read .DEM and .HDR (header)

```c
// points
elevationPointCloud("~/Code/", "W100N90", 41.3110871, -72.8074902, 800, 400, &points, &colors, &numPoints);
//with
glVertexPointer(3, GL_FLOAT, 0, _points);
glColorPointer(3, GL_FLOAT, 0, _colors);
glDrawArrays(GL_POINTS, 0, _numPoints);
```

```c
// filled triangles
elevationTriangles("~/Code/", "W100N90", 41.3110871, -72.8074902, 800, 400, &points, &indices, &colors, &numPoints, &numIndices);
//with
glVertexPointer(3, GL_FLOAT, 0, _points);
glColorPointer(3, GL_FLOAT, 0, _colors);
glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, _indices);
```

#scale

1 world coordinate = 1 km

![tiles image](https://raw.githubusercontent.com/robbykraft/3dEarth/master/sample/newengland.png)

![tiles image](https://raw.githubusercontent.com/robbykraft/3dEarth/master/sample/perspective.png)