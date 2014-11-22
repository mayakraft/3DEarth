#elevation maps

OpenGL mesh rectangles of Earth surface from anywhere in the world using the USGS Global 30 Arc-Second Elevation (GTOPO30) data

![tiles image](https://lta.cr.usgs.gov/sites/default/files/tiles.gif)

#methods

`void elevationPointCloud(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float** points, float** colors)`

* lat/lon mark the center of the plate
* width and height are in km
* filename *without* extension: will read .DEM and .HDR (header)

`elevationPointCloud("~/Code/", "W100N90", 41.3110871, -72.8074902, 800, 400, &points, &colors);`

download tiles for offline use: [ftp://edcftp.cr.usgs.gov/data/gtopo30]

#scale

1 world coordinate = 1 km

![tiles image](https://raw.githubusercontent.com/robbykraft/3dEarth/master/sample/newengland.png)