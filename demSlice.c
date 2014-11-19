//
//  DEM.c
//  Created by Robby on 5/10/14.
//

#include <stdio.h>
#include "demSlice.h"

//TODO: make these dynamic
#define NROWS 6000
#define NCOLS 4800
#define ULXMAP -99.99583333333334
#define ULYMAP 89.99583333333334
#define XDIM 0.00833333333333
#define YDIM 0.00833333333333

unsigned long getByteOffset(float latitude, float longitude){

    float plateWidth = XDIM * NCOLS;  // in degrees, Longitude
    float plateHeight = YDIM * NROWS; // in degrees, Latitude
    
    if(longitude < ULXMAP || longitude > ULXMAP+plateWidth || latitude > ULYMAP || latitude < ULYMAP-plateHeight){
        printf("EXCEPTION: lat long exceeds plate boundary\n");
        return NULL;
    }
    double xOffset = (longitude-ULXMAP)/plateWidth;  // 0.0 - 1.0
    double yOffset = (ULYMAP-latitude)/plateHeight;  // 0.0 - 1.0
    
    unsigned int byteX = xOffset*NCOLS;
    unsigned int byteY = yOffset*NROWS;
    
    return (byteX + byteY*NCOLS) * 2;  // * 2, each index is 2 bytes wide
}


// returns a cropped rectangle from a raw DEM file
// includes edge overflow protection
int16_t* cropFile(FILE *file, unsigned int x, unsigned int y, unsigned int width, unsigned int height){
    //if rectangle overflows past boundary, will move the rectangle and maintain width and height if possible
    //if width or height is bigger than the file's, will shorten the width/height
    if(x > NCOLS || y > NROWS){
        printf("EXCEPTION: starting location lies outside data");
        return NULL;
    }
    if(width > NCOLS){
        width = NCOLS;
        x = 0;
    }
    else if (x+width > NCOLS){
        x -= NCOLS-width;
    }
    if(height > NROWS){
        height = NROWS;
        y = 0;
    }
    else if(y+height > NROWS){
        y -= NROWS-height;
    }
    int16_t *crop = (int16_t*)malloc(sizeof(int16_t)*width*height);
    
//    unsigned long startByte = x*2 + y*NCOLS*2;
    unsigned long startByte = x*2 + y*2*NCOLS;   // (*2) convert byte to uint16
    uint16_t elevation[width];
    int16_t swapped[width];
    fseek(file, startByte, SEEK_SET);
    fread(elevation, sizeof(uint16_t), width, file);
//    for(int i = 0; i < 10; i++)
//        printf("%p  %hu  %u\n",elevation[i], elevation[i], elevation[i]);
//    printf("\n\n\n");
    
    for(int h = 0; h < height; h++){
        fseek(file, startByte+NCOLS*2*h, SEEK_SET);
        fread(elevation, sizeof(uint16_t), width, file);
        // convert from little endian
        for(int i = 0; i < width; i++){
            swapped[i] = (elevation[i]>>8) | (elevation[i]<<8);
            crop[h*width+i] = swapped[i];
        }
        startByte += NCOLS*2;
//        for(int i = 0; i < 10; i++)
//            printf("%p  %hu  %u\n",elevation[i], elevation[i], elevation[i]);
//        for(int i = 0; i < 10; i++)
//            printf("%p  %zd  %d\n",swapped[i], swapped[i], swapped[i]);
    }
    
    return crop;
}

int16_t* elevationForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height){
    
    if(!width || !height)
        return NULL;
    
    unsigned long centerByte = getByteOffset(latitude, longitude);
    unsigned long startByte = centerByte - (width*.5 * 2) - (height*.5 * NCOLS * 2);
    
    printf("%lu, %lu\n", centerByte, startByte);

    int16_t *data = (int16_t*)malloc(sizeof(int16_t)*width*height);
    
    
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
float* elevationPointsForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height){
    if(!width || !height)
        return NULL;
    
    unsigned long centerByte = getByteOffset(latitude, longitude);
    unsigned long startByte = centerByte - (width*.5 * 2) - (height*.5 * NCOLS * 2);
    
    int16_t *data = cropFile(file, centerByte, <#unsigned int y#>, <#unsigned int width#>, <#unsigned int height#>)
    if(data == NULL) return NULL;
    
    printf("%lu, %lu\n", centerByte, startByte);
    
    float *points = (float*)malloc(sizeof(float)*width*height * 3); // x, y, z
    
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
            points[(h*width+i)*3+0] = (i - width*.5) * .01;         // x
            points[(h*width+i)*3+1] = (h - height*.5) * .01;        // y
            points[(h*width+i)*3+2] = swapped[i]/1000.0;    // z, convert meters to km
        }
        
        // increment row
        fseek(file, NCOLS*2, SEEK_CUR);
    }
    return points;
}    


