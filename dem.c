// GTOPO30 elevation data .DEM file processing and OpenGL mesh building
// https://lta.cr.usgs.gov/GTOPO30
//
// Created by Robby Kraft on 5/10/14.
//

#include <stdio.h>

struct demMeta {
    unsigned int nrows;
    unsigned int ncols;
    double ulxmap;
    double ulymap;
    double xdim;
    double ydim;
};

#include "dem.h"

#include <string.h>

struct demMeta loadHeader(char *directory, char *filename){
// looks for .HDR file (packaged with .DEM files from USGS)
    struct demMeta meta;
    
    char path[128];  // you have a directory path larger than 128 chars? must increase this number
    path[0] = '\0';
    printf("%s\n",path);
    strcat(path, directory);
    printf("%s\n",path);
    strcat(path, filename);
    printf("%s\n",path);
    strcat(path, ".HDR");
    printf("%s\n",path);
    FILE *file = fopen(path, "r");
    if(file == NULL){
        printf("\nEXCEPTION: FILE (%s) DOESN'T EXIST\n",path);
        return meta;
    }
    char s1[20], s2[20];
    double d1;
    int i = 0;
    int cmp;
    printf("\nLoading %s\n(%s)\n╔════════════════════════════════\n", filename, directory);
    do {
        cmp = fscanf(file,"%s %lf", s1, &d1);
        if(cmp == 1){
            fscanf(file,"%s", s2);
            printf("║ %s: %s\n", s1, s2);
        }
        else if(cmp > 1){
            printf("║ %s: %f\n", s1, d1);
        }
        if(i == 2) meta.nrows = d1;
        else if(i == 3) meta.ncols = d1;
        else if(i == 10) meta.ulxmap = d1;
        else if(i == 11) meta.ulymap = d1;
        else if(i == 12) meta.xdim = d1;
        else if(i == 13) meta.ydim = d1;
        i++;
    } while (cmp > 0);
    printf("╚════════════════════════════════\n");
    fclose(file);
//    free(path);
    // error on free(), leaking memory in the meantime.
    return meta;
}


void getByteColumnRowFromGeoLocation(struct demMeta meta, float latitude, float longitude, unsigned int *col, unsigned int *row){
    
    double plateWidth = meta.xdim * meta.ncols;  // in degrees, Longitude
    double plateHeight = meta.ydim * meta.nrows; // in degrees, Latitude
    
    if(longitude < meta.ulxmap || longitude > meta.ulxmap+plateWidth || latitude > meta.ulymap || latitude < meta.ulymap-plateHeight){
        printf("\nEXCEPTION: lat long exceeds plate boundary\n");
        *col = -1;
        *row = -1;
        return;
    }
    double xOffset = (longitude-meta.ulxmap)/plateWidth;  // 0.0 - 1.0
    double yOffset = (meta.ulymap-latitude)/plateHeight;  // 0.0 - 1.0
    
    *col = xOffset*meta.ncols;
    *row = yOffset*meta.nrows;
}


unsigned long getByteOffsetFromGeoLocation(struct demMeta meta, float latitude, float longitude){
    unsigned int byteX, byteY;
    getByteColumnRowFromGeoLocation(meta, latitude, longitude, &byteX, &byteY);
    if(byteX == -1) return -1;
    return (byteX + byteY*meta.ncols) * 2;  // * 2, each index is 2 bytes wide
}


int16_t* cropDEMWithMeta(char *directory, char *filename, struct demMeta meta, unsigned int x, unsigned int y, unsigned int width, unsigned int height){
    
    char path[128];  // oh shit you have a directory path larger than 128 chars? i have failed you..
    path[0] = '\0';
    strcat(path, directory);
    strcat(path, filename);
    strcat(path, ".DEM");
    FILE *file = fopen(path, "r");
    int16_t *crop = (int16_t*)malloc(sizeof(int16_t)*width*height);
    if(file == NULL){
        printf("EXCEPTION: FILE (%s) DOESN'T EXIST",path);
        return crop;
    }
    
    unsigned long startByte = x*2 + y*2*meta.ncols;   // (*2) convert byte to uint16
    uint16_t elevation[width];
    int16_t swapped[width];
    fseek(file, startByte, SEEK_SET);
    fread(elevation, sizeof(uint16_t), width, file);
    
//    fseek(file, startByte, SEEK_SET);  //method 2
    for(int h = 0; h < height; h++){
        fseek(file, startByte+meta.ncols*2*h, SEEK_SET);  // method 2 comment this out
        fread(elevation, sizeof(uint16_t), width, file);
        // swap bits: little endian to big
        for(int i = 0; i < width; i++){
            swapped[i] = (elevation[i]>>8) | (elevation[i]<<8);
            crop[h*width+i] = swapped[i];
        }
//        startByte += NCOLS*2;
//        for(int i = 0; i < 10; i++)
//            printf("%p  %hu  %u\n",elevation[i], elevation[i], elevation[i]);
//        for(int i = 0; i < 10; i++)
//            printf("%p  %zd  %d\n",swapped[i], swapped[i], swapped[i]);
//        fseek(file, NCOLS*2, SEEK_CUR); // method 2
    }
    fclose(file);
    return crop;
}


int16_t* cropDEM(char *directory, char *filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height){
    struct demMeta meta = loadHeader(directory, filename);
    return cropDEMWithMeta(directory, filename, meta, x, y, width, height);
}


void checkBoundaries(struct demMeta meta, unsigned int *x, unsigned int *y, unsigned int *width, unsigned int *height){
    //if rectangle overflows past boundary, will move the rectangle and maintain width and height if possible
    //if width or height is bigger than the file's, will shorten the width/height
    if(*x > meta.ncols || *y > meta.nrows){
        printf("\nEXCEPTION: starting location lies outside data\n");
        return;
    }
    if(*width > meta.ncols){
        printf("\nWARNING: width larger than data width, shrinking width to fit\n");
        *width = meta.ncols;
        *x = 0;
    }
    else if (*x+*width > meta.ncols){
        printf("\nWARNING: boundary lies outside data, adjusting origin to fit width\n");
        *x = meta.ncols-*width;
    }
    if(*height > meta.nrows){
        printf("\nWARNING: height larger than data height, shrinking height to fit\n");
        *height = meta.nrows;
        *y = 0;
    }
    else if(*y+*height > meta.nrows){
        printf("\nWARNING: boundary lies outside data, adjusting origin to fit height\n");
        *y = meta.nrows-*height;
    }
}


void elevationPointCloud(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float **points, float **colors, unsigned int *numPoints){
    if(!width || !height)
        return;
    
    // load meta data from header
    struct demMeta meta = loadHeader(directory, filename);
    
    // convert lat/lon into column/row for plate
    unsigned int row, column;
    getByteColumnRowFromGeoLocation(meta, latitude, longitude, &column, &row);
    
    // shift center point to top left, and check boundaries
    column -= width*.5;
    row -= height*.5;
    checkBoundaries(meta, &column, &row, &width, &height);
    printf("Columns:(%d to %d)\nRows:(%d to %d)\n",column, column+width, row, row+height);

    // crop DEM and load it into memory
    int16_t *data = cropDEMWithMeta(directory, filename, meta, column, row, width, height);
    
    // empty point cloud, (x, y, z)
    (*points) = (float*)malloc(sizeof(float) * width*height * 3);
    
    for(int h = 0; h < height; h++){
        for(int w = 0; w < width; w++){
            (*points)[(h*width+w)*3+0] = (w - width*.5);         // x
            (*points)[(h*width+w)*3+1] = (h - height*.5);        // y
            int16_t elev = data[h*width+w];
            if(elev == -9999)
                (*points)[(h*width+w)*3+2] = 0.0f;///1000.0;    // z, convert meters to km
            else
                (*points)[(h*width+w)*3+2] = data[h*width+w];///1000.0;    // z, convert meters to km
        }
    }
    
    (*colors) = (float*)malloc(sizeof(float) * width*height * 3);
    
    for(int i = 0; i < width*height; i++){
        if(data[i] == -9999){
            (*colors)[i*3+0] = 0.0f;
            (*colors)[i*3+1] = 0.24f;
            (*colors)[i*3+2] = 0.666f;
        }
        else if(data[i] > 400){
            float white = (data[i]-400) / 400.0;
        // else if(data[i] > 1200){
        //     float white = (data[i]-1200) / 500.0;
        // else if(data[i] > 900){
        //     float white = (data[i]-900) / 300.0;
            if(white > 1.0f) white = 1.0f;
            (*colors)[i*3+0] = white;
            (*colors)[i*3+1] = 0.3f + 0.7f*white;
            (*colors)[i*3+2] = white;
        }
        else if(data[i] > 100){
            float dark = (data[i]-100) / 300.0;
            if(dark > 1.0f) dark = 1.0f;
            (*colors)[i*3+0] = 0.0f;
            (*colors)[i*3+1] = 0.5f - 0.2f*dark;
            (*colors)[i*3+2] = 0.0f;
        }
        else{
            float orange = (100-data[i]) / 100.0;
            if(orange < 0.0f) orange = 0.0f;
            (*colors)[i*3+0] = orange * .85;
            (*colors)[i*3+1] = 0.5f;
            (*colors)[i*3+2] = 0.0;
        }
    }
    *numPoints = height * width;
}


void elevationTriangles(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float **points, uint32_t **indices, float **colors, unsigned int *numPoints, unsigned int *numIndices){
    if(!width || !height)
        return;
    
    // load meta data from header
    struct demMeta meta = loadHeader(directory, filename);
    
    // convert lat/lon into column/row for plate
    unsigned int row, column;
    getByteColumnRowFromGeoLocation(meta, latitude, longitude, &column, &row);
    
    // shift center point to top left, and check boundaries
    column -= width*.5;
    row -= height*.5;
    checkBoundaries(meta, &column, &row, &width, &height);
    printf("Columns:(%d to %d)\nRows:(%d to %d)\n",column, column+width, row, row+height);

    // crop DEM and load it into memory
    int16_t *data = cropDEMWithMeta(directory, filename, meta, column, row, width, height);
    
    // empty point cloud, (x, y, z)
    (*points) = (float*)malloc(sizeof(float) * width*height * 3);
    
    for(int h = 0; h < height; h++){
        for(int w = 0; w < width; w++){
            (*points)[(h*width+w)*3+0] = (w - width*.5);         // x
            (*points)[(h*width+w)*3+1] = (h - height*.5);        // y
            int16_t elev = data[h*width+w];
            if(elev == -9999)
                (*points)[(h*width+w)*3+2] = 0.0f;///1000.0;    // z, convert meters to km
            else
                (*points)[(h*width+w)*3+2] = data[h*width+w];///1000.0;    // z, convert meters to km
        }
    }

    (*indices) = (uint32_t*)malloc(sizeof(uint32_t) * 2*(width-1)*(height-1) * 3);

    // inside INDICES, (width-1) and (height-1) are max
    // inside POINTS, width and height are max
    for(int h = 0; h < height-1; h++){
        for(int w = 0; w < width-1; w++){
            (*indices)[(h*(width-1)+w)*6+0] = 1*(h*width+w);
            (*indices)[(h*(width-1)+w)*6+1] = 1*((h+1)*width+w);
            (*indices)[(h*(width-1)+w)*6+2] = 1*(h*width+w+1);
            (*indices)[(h*(width-1)+w)*6+3] = 1*((h+1)*width+w);
            (*indices)[(h*(width-1)+w)*6+4] = 1*((h+1)*width+w+1);
            (*indices)[(h*(width-1)+w)*6+5] = 1*(h*width+w+1);
        }
    }

    (*colors) = (float*)malloc(sizeof(float) * width*height * 3);
    
    for(int i = 0; i < width*height; i++){
        if(data[i] == -9999){
            (*colors)[i*3+0] = 0.0f;
            (*colors)[i*3+1] = 0.24f;
            (*colors)[i*3+2] = 0.666f;
        }
        else if(data[i] > 400){
            float white = (data[i]-400) / 400.0;
        // else if(data[i] > 1200){
        //     float white = (data[i]-1200) / 500.0;
        // else if(data[i] > 900){
        //     float white = (data[i]-900) / 300.0;
            if(white > 1.0f) white = 1.0f;
            (*colors)[i*3+0] = white;
            (*colors)[i*3+1] = 0.3f + 0.7f*white;
            (*colors)[i*3+2] = white;
        }
        else if(data[i] > 100){
            float dark = (data[i]-100) / 300.0;
            if(dark > 1.0f) dark = 1.0f;
            (*colors)[i*3+0] = 0.0f;
            (*colors)[i*3+1] = 0.5f - 0.2f*dark;
            (*colors)[i*3+2] = 0.0f;
        }
        else{
            float orange = (100-data[i]) / 100.0;
            if(orange < 0.0f) orange = 0.0f;
            (*colors)[i*3+0] = orange * .85;
            (*colors)[i*3+1] = 0.5f;
            (*colors)[i*3+2] = 0.0;
        }
    }

    *numPoints = height * width;
    *numIndices = 2*(width-1)*(height-1)*3;
}


void elevationTriangleStrip(char *directory, char *filename, float latitude, float longitude, unsigned int width, unsigned int height, float *points, float *colors){
    if(!width || !height)
        return;
    
    // load meta data from header
    struct demMeta meta = loadHeader(directory, filename);
    
    // convert lat/lon into column/row for plate
    unsigned int row, column;
    getByteColumnRowFromGeoLocation(meta, latitude, longitude, &column, &row);
    
    // shift center point to top left, and check boundaries
    column -= width*.5;
    row -= height*.5;
    checkBoundaries(meta, &column, &row, &width, &height);
    printf("Columns:(%d to %d)\nRows:(%d to %d)\n",column, column+width, row, row+height);
    
    // crop DEM and load it into memory
    int16_t *data = cropDEMWithMeta(directory, filename, meta, column, row, width, height);
    
    // empty point cloud, (x, y, z)
    unsigned int count = (width)*2*(height-1) * 3;
    points = (float*)malloc(sizeof(float) * count);
    int16_t elev;
    for(int h = 0; h < height-1; h++){
        for(int q = 0; q < width; q++){
            int w;
            if(h%2 == 0) w = q;
            else         w = width-1-q;
            points[(h*width+w)*6+0] = (w - width*.5);
            points[(h*width+w)*6+1] = (h - height*.5);
            elev = data[h*width+w];
            if(elev == -9999)
                points[(h*width+w)*6+2] = 0.0f;
            else
                points[(h*width+w)*6+2] = data[h*width+w];

            points[(h*width+w)*6+3] = (w - width*.5);
            points[(h*width+w)*6+4] = ((h+1) - height*.5);
            elev = data[(h+1)*width+w];
            if(elev == -9999)
                points[(h*width+w)*6+5] = 0.0f;
            else
                points[(h*width+w)*6+5] = data[(h+1)*width+w];
        }
    }
    // calculate normals
//    for(int i = 1; i < ((height-1)*width*2 - 1); i++){
//        float Ux = points[i*3+0] - points[(i-1)*3+0];
//        float Uy = points[i*3+1] - points[(i-1)*3+1];
//        float Uz = points[i*3+2] - points[(i-1)*3+2];
//        
//        float Vx = points[(i+1)*3+0] - points[(i-1)*3+0];
//        float Vy = points[(i+1)*3+1] - points[(i-1)*3+1];
//        float Vz = points[(i+1)*3+2] - points[(i-1)*3+2];
//        
//        float Nx = Uy*Vz - Uz*Vy;
//        float Ny = Uz*Vx - Ux*Vz;
//        float Nz = Ux*Vy - Uy*Vx;
//
//        points[count+i*3+0] = Nx;
//        points[count+i*3+1] = Ny;
//        points[count+i*3+2] = Nz;
//    }
//        So for a triangle p1, p2, p3, if the vector U = p2 - p1 and the vector V = p3 - p1 then the normal N = U X V and can be calculated by:
//            Nx = UyVz - UzVy
//            Ny = UzVx - UxVz
//            Nz = UxVy - UyVx

}


// IN PROGRESS
//   load political boundary line data
float** loadData(char *directory, char *filename, float **data){

    (*data) = (float*)malloc(sizeof(float)*129 * 2);

    char path[128];  // you have a directory path larger than 128 chars? must increase this number
    path[0] = '\0';
    printf("%s\n",path);
    strcat(path, directory);
    printf("%s\n",path);
    strcat(path, filename);
    printf("%s\n",path);
    FILE *file = fopen(path, "r");
    if(file == NULL){
        printf("\nEXCEPTION: FILE (%s) DOESN'T EXIST\n",path);
        return NULL;
    }
    float f1, f2;
    int i = 0;
    int cmp;
    printf("\nLoading %s\n(%s)\n╔════════════════════════════════\n", filename, directory);
    do {
        cmp = fscanf(file,"%f %f", &f1, &f2);
        printf("║ %f, %f\n", f1, f2);
        //44.0, -120.5
        (*data)[i*2+0] = (f1 + 120.5) * 120;
        (*data)[i*2+1] = -(f2 - 44.0) * 120;
        i++;
    } while (cmp > 0);
    printf("╚════════════════════════════════\n");
    fclose(file);
    return data;
}