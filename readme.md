#elevation maps

.DEM: (GTOPO30) USGS Worldwide 30 Arc-Second Elevation Data (more info: [https://lta.cr.usgs.gov/GTOPO30](https://lta.cr.usgs.gov/GTOPO30))

### .DEM processing tool
* crop tiles
* convert between latitude/longitude and byte offset

### OpenGL mesh builder
* point cloud mesh, triangle mesh
* elevation-based color array

download tiles: [ftp://edcftp.cr.usgs.gov/data/gtopo30](ftp://edcftp.cr.usgs.gov/data/gtopo30)

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