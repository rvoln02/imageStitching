#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

void loadCorners(std::string filename, Corner imageCorners[MAX_CORNERS], unsigned int& numberOfCorners){
    // Read load corners file
    ifstream cornStream;
    ifstream cornCount;
    cornStream.open(filename);
    cornCount.open(filename);
    int counter;
    while(cornCount >> counter){
        numberOfCorners++;
    }
    numberOfCorners /= 2;
    if(!cornStream){
        throw runtime_error("Failed to open " + filename);
    }
    if(numberOfCorners >= MAX_CORNERS){
        throw runtime_error("Too many corners in " + filename);
    }
    Corner corn;
    int x, y;
    int cornIndex = 0;
    while(cornStream >> x >> y){
        corn.x = x;
        corn.y = y;
        imageCorners[cornIndex] = corn;
        cornIndex++;
    }
    cornStream.close();
    cornCount.close();
}

double errorCalculation(Pixel image1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                      Corner image1corner,
                      Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                      Corner image2corner)
{
    Pixel neigh1[ERROR_NEIGHBORHOOD_SIZE][ERROR_NEIGHBORHOOD_SIZE] = {};
    int img1x = image1corner.x;
    int img1y = image1corner.y;
    int img1XEnd = img1x+10;   
    int img1YEnd = img1y+10;

    if((img1y-10 < 0) || (img1x-10 < 0) || (img1XEnd < 0) || (img1YEnd < 0)){
        return INFINITY;
    }

    const unsigned int compHolder1A = img1XEnd;
    const unsigned int compHolder1B = img1YEnd;
    const unsigned int compHolder1C = img1x-10;
    const unsigned int compHolder1D = img1y-10;
    if((compHolder1A > width1) || (compHolder1B > height1) || (compHolder1C > width1) || (compHolder1D > height1)){ //swapping width and height
        return INFINITY;
    }
    

    int neigh1Xindex = 0;
    int neigh1Yindex = 0;
    for(int img1YStart = img1y-10; img1YStart <= img1YEnd;img1YStart++){
        for(int img1XStart = img1x-10;img1XStart <= img1XEnd;img1XStart++){
            neigh1[neigh1Xindex][neigh1Yindex] = image1[img1XStart][img1YStart];
            neigh1Xindex++;
        }
        neigh1Xindex = 0;
        neigh1Yindex++;
    }

    //**********************************************************************************************************************

    Pixel neigh2[ERROR_NEIGHBORHOOD_SIZE][ERROR_NEIGHBORHOOD_SIZE] = {};
    int img2x = image2corner.x;
    int img2y = image2corner.y;

    int img2XEnd = img2x+10;
    int img2YEnd = img2y+10;

    if((img2y-10 < 0) || (img2x-10 < 0) || (img2XEnd < 0) || (img2YEnd < 0)){
        return INFINITY;
    }

    const unsigned int compHolder2A = img2XEnd;
    const unsigned int compHolder2B = img2YEnd;
    const unsigned int compHolder2C = img2x-10;
    const unsigned int compHolder2D = img2y-10;
    if((compHolder2A > width2) || (compHolder2B > height2) || (compHolder2C > width2) || (compHolder2D > height2)){ //swapping height and width
        return INFINITY;
    }

    int neigh2Xindex = 0;
    int neigh2Yindex = 0;
    for(int img2YStart = img2y-10; img2YStart <= img2YEnd;img2YStart++){
        for(int img2XStart = img2x-10;img2XStart <= img2XEnd;img2XStart++){
            neigh2[neigh2Xindex][neigh2Yindex] = image2[img2XStart][img2YStart];
            neigh2Xindex++;
        }
        neigh2Xindex = 0;
        neigh2Yindex++;
    }

    //**********************************************************************************************************************

    Pixel subtracted[ERROR_NEIGHBORHOOD_SIZE][ERROR_NEIGHBORHOOD_SIZE] = {};
    for(size_t y = 0;y < ERROR_NEIGHBORHOOD_SIZE;y++){
        for(size_t x = 0; x < ERROR_NEIGHBORHOOD_SIZE;x++){
            Pixel holdingPix;
            holdingPix.r = (neigh1[x][y].r)-(neigh2[x][y].r);
            holdingPix.g = (neigh1[x][y].g)-(neigh2[x][y].g);
            holdingPix.b = (neigh1[x][y].b)-(neigh2[x][y].b);
            subtracted[x][y] = holdingPix;
        }
    }

    double finError = 0;
     for(size_t y = 0;y < ERROR_NEIGHBORHOOD_SIZE;y++){
        for(size_t x = 0; x < ERROR_NEIGHBORHOOD_SIZE;x++){
            finError += ((abs(subtracted[x][y].r)) + (abs(subtracted[x][y].g)) + (abs(subtracted[x][y].b)));
        }
    }

    return finError;    
}

void matchCorners(CornerPair matchedPairs[MAX_CORNERS], unsigned int &matched_count,
                  Pixel image1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                  Corner image1corners[], unsigned int image1CornerCount,
                  Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  Corner image2corners[], unsigned int image2CornerCount){
    size_t usedCorners[MAX_CORNERS] = {}; //Array to hold used indicies
    size_t usedCornersCount = 0;
    
    int usedIndex;
    for(size_t i = 0;i < image1CornerCount;i++){  //Loop through image1 corners
        double minErr = INFINITY;
        Corner minErrCorner2;  
        for(size_t j = 0;j < image2CornerCount;j++){ //Loop through image2 corners
            bool notIn = true;
            for(size_t k = 0;k < usedCornersCount;k++){
                if(j == usedCorners[k]){
                    notIn = false;
                    break;
                }
            }//check if index has already been used for a comparison
            if(notIn){ 
                if(((errorCalculation(image1,width1,height1,image1corners[i],image2,width2,height2,image2corners[j])) < minErr)){
                    minErr = errorCalculation(image1,width1,height1,image1corners[i],image2,width2,height2,image2corners[j]);
                    minErrCorner2 = image2corners[j];
                    usedIndex = j;

                }   //Updates the minimum error value and stores the corresponding image2 corner and index 
            }
        }
        if(minErr < INFINITY){
                CornerPair matched;
                matched.image1Corner = image1corners[i];
                matched.image2Corner = minErrCorner2;
                matched.error = minErr; 

                matchedPairs[matched_count] = matched;  //Puts matched pair into matched pair array
                matched_count++;

                usedCorners[usedCornersCount] = usedIndex;  //Puts used index into used index array
                usedCornersCount++;
        }
    }   
}


void mapCoordinates(const double H[3][3], unsigned int x, unsigned int y,
                    double& mapped_x, double& mapped_y){
    // Mapping function for this week
    double xPrime = ((H[0][0])*x) + ((H[0][1])*y) + (H[0][2]);
    double yPrime = ((H[1][0])*x) + ((H[1][1])*y) + (H[1][2]);
    double zPrime = ((H[2][0])*x) + ((H[2][1])*y) + (H[2][2]);
    mapped_x = xPrime/zPrime;
    mapped_y = yPrime/zPrime;
}


void mergeImages( Pixel image1[][MAX_HEIGHT], unsigned int &width1, unsigned int &height1,
                  Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  double H[3][3] ){
    // Here to stop usless warning 
    Pixel warningDeleter = image2[0][0];
    short thing = 2;
    warningDeleter.r = 2;
    thing += warningDeleter.r;


    Pixel buffer[MAX_WIDTH][MAX_HEIGHT];
    for(size_t row = 0; row < height1;row++){
        for(size_t col = 0;col < width1;col++){
            buffer[col][row] = image1[col][row];
        }
    }

    for(size_t row = 0; row < MAX_HEIGHT;row++){
        for(size_t col = 0;col < MAX_WIDTH;col++){
            double mapX;
            double mapY;
            Pixel I2;
            Pixel I1;
            mapCoordinates(H,col,row,mapX,mapY);

            bool I2def = false;
            if(((mapX >= 0) && (mapX < width2)) && ((mapY >= 0) && (mapY < height2))){
                I2 = bilinear_interpolation(buffer,width1,height1,mapX,mapY);
                I2def = true;
            }
            bool I1def = false;
            if(((col < width1)) && (row < height1)){
                I1 = buffer[col][row];
                I1def = true;
            }

            Pixel newPix;
            
            if((!I1def) && (!I2def)){
                newPix.r = 0;
                newPix.g = 0;
                newPix.b = 0;
            }
            
            if((!I2def) && (I1def)){
                newPix = I1;
            }
            if((!I1def) && (I2def)){
                newPix = I2;
            }
            if((I1def) && (I2def)){
                newPix.r = ((I1.r + I2.r) / 2);
                newPix.g = ((I1.g + I2.g) / 2);
                newPix.b = ((I1.b + I2.b) / 2);
            }
            image1[col][row] = newPix;
        }
    }
    width1 = MAX_WIDTH;
    height1 = MAX_HEIGHT;
}