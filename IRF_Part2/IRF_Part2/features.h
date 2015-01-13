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

int featureHistogram(Mat im, int tab[]);
int featureBW(Mat im, int tab[]);
int featureNbHorizontal(Mat im, int tab[]);
int featureNbVertical(Mat im, int tab[]);

#endif // IMAGETTE_H
