#ifndef NORMALIZATION
#define NORMALIZATION
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

Mat BoundingRatio(Mat im);

#endif // NORMALIZATION

