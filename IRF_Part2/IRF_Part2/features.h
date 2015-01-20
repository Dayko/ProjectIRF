#ifndef FEATURES_H
#define FEATURES_H
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>

#include "cv.h"
#include "highgui.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace std;
using namespace cv;

int featureHistogram(Mat im, float tab[]);
int featureBW(Mat im, float tab[]);
int featureBoundingRatio(Mat im, float tab[], bool divided);
int featureNbBlackPixelLinesCols(Mat im, float tab[]);
int featureHoughLines(Mat im, float tab[]);
int featureHoughCircles(Mat im, float tab[]);
int featureGravityCenter(Mat im, float tab[]);
int featureCannyEdge(Mat im, float tab[]);

#endif // IMAGETTE_H
