//
//  DEM.c
//  Created by Robby on 5/10/14.
//

#include <stdio.h>
#include "DEM.h"

//TODO: make these dynamic
#define NROWS 6000
#define NCOLS 4800
#define ULXMAP -139.99583333333334
#define ULYMAP 89.99583333333334
#define XDIM 0.00833333333333
#define YDIM 0.00833333333333

unsigned long getByteOffset(float latitude, float longitude){

    float plateWidth = XDIM * NCOLS;  // in degrees, Longitude
    float plateHeight = YDIM * NROWS; // in degrees, Latitude
    
    if(longitude < ULXMAP || longitude > ULXMAP+plateWidth || latitude > ULYMAP || latitude < ULYMAP-plateHeight){
        printf("ERROR: lat long exceeds plate boundary\n");
        return 0;
    }
    double xOffset = (longitude-ULXMAP)/plateWidth;  // 0.0 - 1.0
    double yOffset = (ULYMAP-latitude)/plateHeight;  // 0.0 - 1.0
    
    unsigned int byteX = xOffset*NCOLS;
    unsigned int byteY = yOffset*NROWS;
    
    return (byteX + byteY*NCOLS) * 2;  // * 2, each index is 2 bytes wide
}

int16_t* elevationForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height){
    
    if(!width || !height)
        return NULL;
    
    unsigned long centerByte = getByteOffset(latitude, longitude);
    unsigned long startByte = centerByte - (width*.5 * 2) - (height*.5 * NCOLS * 2);
    
    printf("%lu, %lu\n", centerByte, startByte);

    int16_t *data = malloc(sizeof(int16_t)*width*height);
    
    
    printf("offset: %lu\n",startByte);
    
    uint16_t elevation[width];
    int16_t swapped[width];
    
    for(int h = 0; h < height; h++){
        
        fseek(file, startByte, SEEK_SET);
        fread(elevation, sizeof(uint16_t), width, file);
        
        for(int i = 0; i < width; i++){
            swapped[i] = (elevation[i]>>8) | (elevation[i]<<8);
            data[h*width+i] = swapped[i];
        }
        startByte += NCOLS*2;
//        for(int i = 0; i < 10; i++)
//            printf("%p  %hu  %u\n",elevation[i], elevation[i], elevation[i]);
//        for(int i = 0; i < 10; i++)
//            printf("%p  %zd  %d\n",swapped[i], swapped[i], swapped[i]);
    }
//        fseek(file, 100, SEEK_SET); // seek to the 100th byte of the file
//        fseek(file, -30, SEEK_CUR); // seek backward 30 bytes from the current pos
//        fseek(file, -10, SEEK_END); // seek to the 10th byte before the end of file
    return data;
}

// point cloud
//X:longitude Y:latitude Z:elevation
float* PointsForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height){
    if(!width || !height)
        return NULL;
    
    unsigned long centerByte = getByteOffset(latitude, longitude);
    unsigned long startByte = centerByte - (width*.5 * 2) - (height*.5 * NCOLS * 2);
    
    printf("%lu, %lu\n", centerByte, startByte);
    
    float *points = malloc(sizeof(float)*width*height * 3); // x, y, z
    
    printf("offset: %lu\n",startByte);
    
    uint16_t elevation[width];
    int16_t swapped[width];
    
    fseek(file, startByte, SEEK_SET);
    
    for(int h = 0; h < height; h++){
        
        // read a row, size of width
        fseek(file, startByte, SEEK_SET);
        fread(elevation, sizeof(uint16_t), width, file);
        
        // process each column
        for(int i = 0; i < width; i++){
            swapped[i] = (elevation[i]>>8) | (elevation[i]<<8);
            points[(h*width+i)*3+0] = i - width*.5;         // x
            points[(h*width+i)*3+1] = h - height*.5;        // y
            points[(h*width+i)*3+2] = swapped[i]/1000.0;    // z, convert meters to km
        }
        
        // increment row
        fseek(file, NCOLS*2, SEEK_CUR);
    }
    return points;
}    


