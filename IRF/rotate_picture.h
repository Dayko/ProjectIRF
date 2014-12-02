#ifndef ROTATE_PICTURE_H
#define ROTATE_PICTURE_H

#include "cv.h"
#include "highgui.h"
#include "basicMatMethods.h"
#include "time.h"

#define MAXNBMATCHES 5
#define NONDETECTIONLIMIT 0.2

using namespace cv;
using namespace std;


int rotatePicture(Mat src_org);

Mat getRotatedImage(Mat src, int angle);

double getMinThresholdValue(Mat ref, Mat src, int numberOfMatches);

float euclideanDist(Point& p, Point& q);

int findCorrectAngle(Mat src);

Mat getImageCorrectlyRotated(string path);

void getSmallPicRef(Mat src_rot, Mat src_rot_refPic[]);

void getCorrespondanceToRefPic(Mat reference_Pic_RGB[], Mat src_rot_refPic[], int correspondant_Ref_Pic[]);

double getPointsFromRefImage(Mat ref, Mat src, Point points[]);

double getFineAngle(Mat src);

Mat getRotatedImagedouble(Mat src, double angle);



#endif // ROTATE_PICTURE_H
