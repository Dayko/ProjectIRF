#ifndef BASICMATMETHODS_H
#define BASICMATMETHODS_H


#include "cv.h"
#include "highgui.h"

#define NBROW 7
#define NBCOLUMN 5
#define NBICONREF 14
#define NONDETECTIONLIMIT 0.2

using namespace cv;
using namespace std;

Mat loadImage(string imName);

Mat getSmallerImage(Mat inputImage, int sizeReduction);

void loadReferenceImages(string inputPaths[], Mat outputTMat[], int nb);

void splitImage(Mat src, Mat output[], int height);

double getPointsFromRefImage(Mat ref, Mat src, Point points[], int MaxNumberOfMatches);
float euclideanDist(Point& p, Point& q);

bool isThereMatchs(Mat ref, Mat src, double thresholdMinValue);

#endif // BASICMATMETHODS_H
