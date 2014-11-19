//
//  DEM.c
//  Created by Robby on 5/10/14.
//

#include <stdio.h>
#include <string>
#include "dem.h"

// DEM standard for all plates except Antartica
//#define NROWS 6000
//#define NCOLS 4800
//#define XDIM 0.00833333333333
//#define YDIM 0.00833333333333
//#define ULXMAP -89.3333333
//#define ULYMAP 90.0

struct demMeta {
    unsigned int nrows;
    unsigned int ncols;
    double ulxmap;
    double ulymap;
    double xdim;
    double ydim;
};
typedef struct demMeta demMeta;

demMeta loadHeader(string directory, string filename){
// coordinate to .HDR file standard, accompanies .DEM files
    demMeta meta;
    string path = directory+filename+".HDR";
    FILE *file = fopen(path.c_str(), "r");
    char s1[20], s2[20];
    double d1;
    int i = 0;
    int cmp;
    do {
        cmp = fscanf(file,"%s %lf", s1, &d1);
        if(cmp == 1){
            fscanf(file,"%s", s2);
            printf("%s: %s\n", s1, s2);
        }
        else if(cmp > 1){
            printf("%s: %f\n", s1, d1);
        }
        if(i == 2) meta.nrows = d1;
        else if(i == 3) meta.ncols = d1;
        else if(i == 10) meta.ulxmap = d1;
        else if(i == 11) meta.ulymap = d1;
        else if(i == 12) meta.xdim = d1;
        else if(i == 13) meta.ydim = d1;
        i++;
    } while (cmp > 0);
    fclose(file);
    return meta;
}

void loadDEM(string directory, string file){
    
}

unsigned long getByteOffset(float latitude, float longitude, demMeta meta){
    
    float plateWidth = meta.xdim * meta.ncols;  // in degrees, Longitude
    float plateHeight = meta.ydim * meta.nrows; // in degrees, Latitude
    
    if(longitude < meta.ulxmap || longitude > meta.ulxmap+plateWidth || latitude > meta.ulymap || latitude < meta.ulymap-plateHeight){
        printf("\nEXCEPTION: lat long exceeds plate boundary\n");
        return NULL;
    }
    double xOffset = (longitude-meta.ulxmap)/plateWidth;  // 0.0 - 1.0
    double yOffset = (meta.ulymap-latitude)/plateHeight;  // 0.0 - 1.0
    
    unsigned int byteX = xOffset*meta.ncols;
    unsigned int byteY = yOffset*meta.nrows;
    
    return (byteX + byteY*meta.ncols) * 2;  // * 2, each index is 2 bytes wide
}

void convertLatLonToXY(demMeta meta, float latitude, float longitude, unsigned int *x, unsigned int *y){
    
    double plateWidth = meta.xdim * meta.ncols;  // in degrees, Longitude
    double plateHeight = meta.ydim * meta.nrows; // in degrees, Latitude
    
    if(longitude < meta.ulxmap || longitude > meta.ulxmap+plateWidth || latitude > meta.ulymap || latitude < meta.ulymap-plateHeight){
        printf("\nEXCEPTION: lat long exceeds plate boundary\n");
        return;
    }
    double xOffset = (longitude-meta.ulxmap)/plateWidth;  // 0.0 - 1.0
    double yOffset = (meta.ulymap-latitude)/plateHeight;  // 0.0 - 1.0
    
    printf("%f, %f\n",xOffset, yOffset);
    
    *x = xOffset*meta.ncols;
    *y = yOffset*meta.nrows;
}


//unsigned long getByteOffset(float latitude, float longitude){
//
//    float plateWidth = XDIM * NCOLS;  // in degrees, Longitude
//    float plateHeight = YDIM * NROWS; // in degrees, Latitude
//    
//    if(longitude < ULXMAP || longitude > ULXMAP+plateWidth || latitude > ULYMAP || latitude < ULYMAP-plateHeight){
//        printf("\nEXCEPTION: lat long exceeds plate boundary\n");
//        return NULL;
//    }
//    double xOffset = (longitude-ULXMAP)/plateWidth;  // 0.0 - 1.0
//    double yOffset = (ULYMAP-latitude)/plateHeight;  // 0.0 - 1.0
//    
//    unsigned int byteX = xOffset*NCOLS;
//    unsigned int byteY = yOffset*NROWS;
//    
//    return (byteX + byteY*NCOLS) * 2;  // * 2, each index is 2 bytes wide
//}
//
//void startOffset(float latitude, float longitude, unsigned int *x, unsigned int *y){
//    
//    double plateWidth = XDIM * NCOLS;  // in degrees, Longitude
//    double plateHeight = YDIM * NROWS; // in degrees, Latitude
//    
//    if(longitude < ULXMAP || longitude > ULXMAP+plateWidth || latitude > ULYMAP || latitude < ULYMAP-plateHeight){
//        printf("\nEXCEPTION: lat long exceeds plate boundary\n");
//        return;
//    }
//    double xOffset = (longitude-ULXMAP)/plateWidth;  // 0.0 - 1.0
//    double yOffset = (ULYMAP-latitude)/plateHeight;  // 0.0 - 1.0
//    
//    printf("%f, %f\n",xOffset, yOffset);
//    
//    *x = xOffset*NCOLS;
//    *y = yOffset*NROWS;
//}

// returns a cropped rectangle from a raw DEM file
// includes edge overflow protection
// rect defined by (x,y):top left corner and width, height
int16_t* cropDEM(string directory, string filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height){
    //if rectangle overflows past boundary, will move the rectangle and maintain width and height if possible
    //if width or height is bigger than the file's, will shorten the width/height
    
    demMeta meta = loadHeader(directory, filename);
    string path = directory + filename + ".DEM";
    FILE *file = fopen(path.c_str(), "r");
    printf("%d :: %d\n",x, y);
    if(x > meta.ncols || y > meta.nrows){
        printf("EXCEPTION: starting location lies outside data");
        return NULL;
    }
    if(width > meta.ncols){
        width = meta.ncols;
        x = 0;
    }
    else if (x+width > meta.ncols){
        x -= meta.ncols-width;
    }
    if(height > meta.nrows){
        height = meta.nrows;
        y = 0;
    }
    else if(y+height > meta.nrows){
        y -= meta.nrows-height;
    }
    int16_t *crop = (int16_t*)malloc(sizeof(int16_t)*width*height);
    
//    unsigned long startByte = x*2 + y*NCOLS*2;
    unsigned long startByte = x*2 + y*2*meta.ncols;   // (*2) convert byte to uint16
    uint16_t elevation[width];
    int16_t swapped[width];
    fseek(file, startByte, SEEK_SET);
    fread(elevation, sizeof(uint16_t), width, file);
//    for(int i = 0; i < 10; i++)
//        printf("%p  %hu  %u\n",elevation[i], elevation[i], elevation[i]);
//    printf("\n\n\n");
    
//    fseek(file, startByte, SEEK_SET);  //method 2
    for(int h = 0; h < height; h++){
        fseek(file, startByte+meta.ncols*2*h, SEEK_SET);  // method 2 comment this out
        fread(elevation, sizeof(uint16_t), width, file);
        // convert from little endian
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

//int16_t* elevationForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height){
//    
//    if(!width || !height)
//        return NULL;
//    
//    unsigned long centerByte = getByteOffset(latitude, longitude);
//    unsigned long startByte = centerByte - (width*.5 * 2) - (height*.5 * NCOLS * 2);
//    
//    printf("%lu, %lu\n", centerByte, startByte);
//
//    int16_t *data = (int16_t*)malloc(sizeof(int16_t)*width*height);
//    
//    
//    printf("offset: %lu\n",startByte);
//    
//    uint16_t elevation[width];
//    int16_t swapped[width];
//    
//    for(int h = 0; h < height; h++){
//        
//        fseek(file, startByte, SEEK_SET);
//        fread(elevation, sizeof(uint16_t), width, file);
//        
//        for(int i = 0; i < width; i++){
//            swapped[i] = (elevation[i]>>8) | (elevation[i]<<8);
//            data[h*width+i] = swapped[i];
//        }
//        startByte += NCOLS*2;
////        for(int i = 0; i < 10; i++)
////            printf("%p  %hu  %u\n",elevation[i], elevation[i], elevation[i]);
////        for(int i = 0; i < 10; i++)
////            printf("%p  %zd  %d\n",swapped[i], swapped[i], swapped[i]);
//    }
////        fseek(file, 100, SEEK_SET); // seek to the 100th byte of the file
////        fseek(file, -30, SEEK_CUR); // seek backward 30 bytes from the current pos
////        fseek(file, -10, SEEK_END); // seek to the 10th byte before the end of file
//    return data;
//}

float* elevationPointsFromDEM(string directory, string filename, float latitude, float longitude, unsigned int width, unsigned int height){
    if(!width || !height)
        return NULL;
    
    // load meta data from header
    demMeta meta = loadHeader(directory, filename);

    // convert lat/lon into column/row for plate
    unsigned int row, column;
    convertLatLonToXY(meta, latitude, longitude, &column, &row);
    printf("Row: %d    Column: %d\n",row, column);

    // crop DEM and load it into memory
    int16_t *data = cropDEM(directory, filename, column-width*.5, row-height*.5, width, height);

    float *points = (float*)malloc(sizeof(float)*width*height * 3); // x, y, z
    
    for(int h = 0; h < height; h++){
        for(int w = 0; w < width; w++){
            points[(h*width+w)*3+0] = (w - width*.5);         // x
            points[(h*width+w)*3+1] = (h - height*.5);        // y
            int16_t elev = data[h*width+w];
            if(elev == -9999)
                points[(h*width+w)*3+2] = 0.0f;///1000.0;    // z, convert meters to km
            else
                points[(h*width+w)*3+2] = data[h*width+w];///1000.0;    // z, convert meters to km
        }
    }
    return points;
}

// point cloud
//X:longitude Y:latitude Z:elevation
//float* elevationPointsForArea(FILE *file, float latitude, float longitude, unsigned int width, unsigned int height){
//    if(!width || !height)
//        return NULL;
//    
//    unsigned long centerByte = getByteOffset(latitude, longitude);
//    unsigned long startByte = centerByte - (width*.5 * 2) - (height*.5 * NCOLS * 2);
//    unsigned int row, column;
//    startOffset(latitude, longitude, &column, &row);
//    printf("%d : %d\n",row, column);
//    int16_t *data = cropFile(file, column, row, width, height);
//    if(data == NULL) return NULL;
//    
//    printf("%lu, %lu\n", centerByte, startByte);
//    
//    float *points = (float*)malloc(sizeof(float)*width*height * 3); // x, y, z
//    
//    for(int h = 0; h < height; h++){
//        for(int w = 0; w < width; w++){
//            points[(h*width+w)*3+0] = (w - width*.5);         // x
//            points[(h*width+w)*3+1] = (h - height*.5);        // y
//            int16_t elev = data[h*width+w];
//            if(elev == -9999)
//                points[(h*width+w)*3+2] = 0.0f;///1000.0;    // z, convert meters to km
//            else
//                points[(h*width+w)*3+2] = data[h*width+w];///1000.0;    // z, convert meters to km
//        }
//    }
//    return points;
//}


