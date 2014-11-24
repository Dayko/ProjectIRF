#ifndef BASICMATMETHODS_H
#define BASICMATMETHODS_H


#include "cv.h"
#include "highgui.h"

#define NBROW 7
#define NBCOLUMN 6
#define NBICONREF 14

using namespace cv;
using namespace std;

Mat loadImage(string imName);

Mat getSmallerImage(Mat inputImage, int sizeReduction);

void loadReferenceImages(string inputPaths[], Mat outputTMat[], int nb);

void splitImage(Mat src, Mat output[], int height);

#endif // BASICMATMETHODS_H
