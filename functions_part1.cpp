#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

// Copy code from last week for local run. Gradescope will use it's own copy

void loadImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height) {
  ifstream fileStream;
  ifstream counterStream; //second stream for main purpose of counting RGB values
  fileStream.open(filename);
  counterStream.open(filename); 
  //If file doesn't open throw error
  if(!fileStream){
    throw runtime_error("Failed to open " + filename);
  }
  
  string preamble;
  int max; //holds the max pixel amount (which is not needed for our purposes)
  fileStream >> preamble >> width >> height >> max;

  string filler1; //Too collect useless parts before counting RGB values
  double filler2;
  double filler3;
  int filler4;
  counterStream >> filler1 >> filler2 >> filler3 >> filler4;
  int counterInt;
  int pixCount = 0;
  while(counterStream >> counterInt){
    pixCount++;
  }

  //Checking if dimensions are not floats
  if((ceil(filler2) != filler2) || (ceil(filler3) != filler3)){
    throw runtime_error("Invald dimensions");
  } 
  //Checking if dimensions are positive and less than max values
  if(((width > MAX_WIDTH) || (width <= 0)) || ((height > MAX_HEIGHT) || (height <= 0))){
    throw runtime_error("Invald dimensions");
  }
  //Check if preamble is correct type
  if((preamble != "P3") && (preamble != "p3")){
    throw runtime_error("Invalid data type " + preamble);
  }
  

  int intend = width * height * 3; //intended number of RGB values
  if(pixCount < intend){
    throw runtime_error("Invalid color value");
  }
  if(pixCount > intend){
    throw runtime_error("Too many values");
  }
  
  
  for(size_t row = 0; row < height;row++){
    for(size_t col = 0;col < width;col++){
      int red;
      int green;
      int blue;
      Pixel pix;
      fileStream >> red >> green >> blue;

      //Checking if individual color values are valid
      if(((red >= 256) || (red < 0)) || ((green >= 256) || (green < 0)) || ((blue >= 256) || (blue < 0))){
        throw runtime_error("Invalid color value");
      }

      //Puts values into pixel 
      pix.r = red;
      pix.g = green;
      pix.b = blue;
      image[col][row] = pix;
    }
  }
  fileStream.close();
  counterStream.close();
}


void outputImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height) {
  ofstream fileWriter;
  fileWriter.open(filename);
  if(!fileWriter){
    throw runtime_error("Failed to open " + filename);
  }
  fileWriter << "P3\n";
  fileWriter << width << " " << height << "\n";
  fileWriter << 255 << "\n";

  for(size_t row = 0; row < height;row++){
    for(size_t col = 0;col < width;col++){
      fileWriter << (image[col][row]).r << " " << (image[col][row]).g << " " << (image[col][row]).b << " ";
    }
    fileWriter << "\n";
  }
  fileWriter.close();
}


Pixel bilinear_interpolation(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, double x, double y) {
  /*
  This is here because theres a warning for unused parameters of width and height, 
  and I don't need to use them for my program to work.
  */
  int wowza = width*height;
  wowza++;

  int ceilX = ceil(x);
  int floorX = floor(x);
  int ceilY = ceil(y);
  int floorY = floor(y);

  if(ceilX == floorX){
    ceilX += 1;
  }
  if(ceilY == floorY){
    ceilY += 1;
  }
  Pixel P1 = image[floorX][floorY];
  Pixel P2 = image[ceilX][floorY];
  Pixel P3 = image[floorX][ceilY];
  Pixel P4 = image[ceilX][ceilY];

  
  double I1r = (x-floorX) * P2.r + (ceilX - x) * P1.r;
  double I1g = (x-floorX) * P2.g + (ceilX - x) * P1.g;
  double I1b = (x-floorX) * P2.b + (ceilX - x) * P1.b;

  
  double I2r = (x-floorX) * P4.r + (ceilX - x) * P3.r;
  double I2g = (x-floorX) * P4.g + (ceilX - x) * P3.g;
  double I2b = (x-floorX) * P4.b + (ceilX - x) * P3.b;

  Pixel fin;
  fin.r = round((ceilY - y) * I1r + (y - floorY) * I2r);
  fin.g = round((ceilY - y) * I1g + (y - floorY) * I2g);
  fin.b = round((ceilY - y) * I1b + (y - floorY) * I2b);
  return fin;
}

