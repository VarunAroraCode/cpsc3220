//Name:         
//Instructor:   Brygg Ullmer
//Class:        3220
//Date:         

// Base code by Bidur Bohara (LSU) in collaboration with Brygg Ullmer

//#include <QCoreApplication>
#include <vector>
#include <math.h>
#include <thread>
#include "cscbitmap.h"
#include <stdio.h>

int sobel_x[3][3] = { { 1, 0,-1},
                      { 2, 0,-2},
                      { 1, 0,-1}};

int sobel_y[3][3] = { { 1, 2, 1},
                      { 0, 0, 0},
                      {-1,-2,-1}};

/// Declaration of functions.
void* findEdge(const unsigned int w, const unsigned int h, unsigned int widthStart, unsigned int widthEnd);

/// Memory to hold input image data
unsigned char* inData;
std::vector<unsigned char> image_sobeled;

// Qt entry point!
// Create and run a thread inside main function.
// Also assign region of image to a thread, and 
// call thread join for synchronization.
int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    char* bmpFile;
    char* charThreads;
    if( argc < 3)
      {
        printf("Filename argument required!\n");
        return 0;
      }
    else{
      bmpFile = argv[1]; 
      charThreads = argv[2];
    }
    int numThreads = atoi(charThreads);
    /// Open and read bmp file.
    Bitmap *image = new Bitmap();
    unsigned char*data = image->readGrayBitmap(bmpFile);

    image_sobeled.clear();
    image_sobeled.resize(image->bmpSize, 255);
    inData = data;
    //mythread(findEdge, image->bmpWidth, image->bmpHeight);

    //creating variables to manipulate
    int height = image->bmpHeight;
    int width = image->bmpWidth;
    //divide height and width into chunks and run each chunk
    //on its own thread.

    //This int will give me the width of each segmented image
    //after it has been split depending on the number of threads
    unsigned int threadWidth = width/numThreads;
    //will keep track of which current segment the findEdge is looking at
    unsigned int counter = 0;
    //pushing all threads onto vector to join for later
    std::vector<std::thread> vectorThread;
    for(int i = 0; i < numThreads; i++){
      //creating a new thread and having it call findEdge with segmented data
      std::thread newThread(findEdge, width, height, counter, counter + threadWidth);
      //pushing that thread onto a vector to join for later
      vectorThread.push_back(move(newThread));
      //keeps track of which segment of the bitmap is currently being worked on
      counter += threadWidth;
    }
    //making a join for each thread in the vector of threads 
    for(int i = 0; i < numThreads; i++){
      vectorThread[i].join();
    }
    /// Write image data passed as argument to a bitmap file
    image->writeGrayBmp(&image_sobeled[0]);
    image_sobeled.clear();
    delete data;
    //mythread.join();

    return 0; 
    //return a.exec();
}


/// Function that implements Sobel operator.
/// Returns image data after applying Sobel operator to the original image.
/// Reimplement findEdge such that it will run in a single thread
/// and can process on a region/group of pixels
void* findEdge(const unsigned int w,
	       const unsigned int h, unsigned int widthStart, unsigned int widthEnd)
{
    int gradient_X = 0;
    int gradient_Y = 0;
    int value = 0;

    // The FOR loop apply Sobel operator
    // to bitmap image data in per-pixel level.
    for(unsigned int y = 1; y < h-1; ++y)
        for(unsigned int x = widthStart; x < widthEnd; ++x)
        {
            // Compute gradient in +ve x direction
            gradient_X = sobel_x[0][0] * inData[ (x-1) + (y-1) * w ]
                    + sobel_x[0][1] * inData[  x    + (y-1) * w ]
                    + sobel_x[0][2] * inData[ (x+1) + (y-1) * w ]
                    + sobel_x[1][0] * inData[ (x-1) +  y    * w ]
                    + sobel_x[1][1] * inData[  x    +  y    * w ]
                    + sobel_x[1][2] * inData[ (x+1) +  y    * w ]
                    + sobel_x[2][0] * inData[ (x-1) + (y+1) * w ]
                    + sobel_x[2][1] * inData[  x    + (y+1) * w ]
                    + sobel_x[2][2] * inData[ (x+1) + (y+1) * w ];

            // Compute gradient in +ve y direction
            gradient_Y = sobel_y[0][0] * inData[ (x-1) + (y-1) * w ]
                    + sobel_y[0][1] * inData[  x    + (y-1) * w ]
                    + sobel_y[0][2] * inData[ (x+1) + (y-1) * w ]
                    + sobel_y[1][0] * inData[ (x-1) +  y    * w ]
                    + sobel_y[1][1] * inData[  x    +  y    * w ]
                    + sobel_y[1][2] * inData[ (x+1) +  y    * w ]
                    + sobel_y[2][0] * inData[ (x-1) + (y+1) * w ]
                    + sobel_y[2][1] * inData[  x    + (y+1) * w ]
                    + sobel_y[2][2] * inData[ (x+1) + (y+1) * w ];

            value = (int)ceil(sqrt( gradient_X * gradient_X +
                                    gradient_Y * gradient_Y));
            image_sobeled[ x + y*w ] = 255 - value;
        }
    // Visual Studio requires this to be present; and should not 
    // cause issues for other compilers. 
    // Thanks to Thomas Peters.
    return 0;
}
