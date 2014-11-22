#elevation maps

OpenGL mesh rectangles of Earth surface from anywhere in the world using the USGS Global 30 Arc-Second Elevation (GTOPO30) data

![tiles image](https://lta.cr.usgs.gov/sites/default/files/tiles.gif)

#methods

`elevationPointCloud("/path/", "file", latitude, longitude, width, height)`

* lat/lon mark the center of the plate
* width and height are in km

#usage

download tiles for offline use: [ftp://edcftp.cr.usgs.gov/data/gtopo30]

```
pointCloud = elevationPointCloud("/path/", "W100N90", 40.7110871, -73.3074902, 100, 100);

glVertexPointer(3, GL_FLOAT, 0, pointCloud);
glDrawArrays(GL_POINTS, 0, 10000);
```

#scale

1 world coordinate = 1 km