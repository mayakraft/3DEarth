#elevation maps

OpenGL mesh rectangles of Earth surface from anywhere in the world using the USGS Global 30 Arc-Second Elevation (GTOPO30) data

![tiles image](https://lta.cr.usgs.gov/sites/default/files/tiles.gif)

#usage

download tiles for offline, local use: [ftp://edcftp.cr.usgs.gov/data/gtopo30]

`FILE *file = fopen("/w100n90/W100N90.DEM", "r");`

define rectangle by center of the rect (lon, lat), then total width (lon), height (lat)

#scale

1 world coordinate = 1 km