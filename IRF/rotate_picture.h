#ifndef ROTATE_PICTURE_H
#define ROTATE_PICTURE_H

#include "cv.h"
#include "highgui.h"
#include "basicMatMethods.h"
#include "time.h"

#define NONDETECTIONLIMIT 0.05

using namespace cv;
using namespace std;


double getFineAngle(Mat src, Mat ref);
int getMajorAngle(Mat src, Mat ref);
Mat getRotatedImage(Mat src, double angle);
Mat getImageCorrectlyRotated(string path, Mat refCrosses);



#endif // ROTATE_PICTURE_H
