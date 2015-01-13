#ifndef FEATURES_H
#define FEATURES_H
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>

#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

int featureHistogram(Mat im, float tab[]);
int featureBW(Mat im, float tab[]);
int featureNbHorizontal(Mat im, float tab[]);
int featureNbVertical(Mat im, float tab[]);
int featureBoundingRatio(Mat im, float tab[]);
int featureNbBlackPixelLinesCols(Mat im, float tab[]);
int featureHoughLines(Mat im, float tab[]);
int featureHoughCircles(Mat im, float tab[]);

#endif // IMAGETTE_H
