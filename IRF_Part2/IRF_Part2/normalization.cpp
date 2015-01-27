#include "normalization.h"

Mat BoundingRatio(Mat inputImage){
    int areaMin;
    int areaMax;

    areaMin = 2000;
    areaMax = 57800;
	Mat im;
	inputImage.copyTo(im);
    // Finds contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(im, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));

    // Approximate contours to polygons + get bounding rects
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    for (int i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }

    vector<Rect> rectangleToMerge;
    for (int i = 0; i < boundRect.size(); i++)
    {
        //test of the Rectangle :
        // If it's approximatly equal to the dimension, in height or width, of the whole image, then it's probably the box of the whole image
        // or it's a wrong box wich result of a remaining border of an imagette. We also check it if the ratio of the box is anormal (too small)
        // We also discard too small rectangles and too big (another check for the whole image bouding box)

        if (/*!((im.size().height - boundRect[i].height) < im.size().height*0.02)
                && !((im.size().width - boundRect[i].width) < im.size().width*0.02)
                &&*/ boundRect[i].area() < areaMax && boundRect[i].area() > areaMin
                && !(((float)(boundRect[i].height) / (float)(boundRect[i].width)) < 0.05 || ((float)(boundRect[i].width) / (float)(boundRect[i].height)) < 0.05)){
            rectangleToMerge.push_back(boundRect[i]);
        }
    }
    /* Impression de des bounding boxes que l'on considère valides */
    double ymin=10000, ymax=0, xmin=10000, xmax=0;
    for (int i = 0; i < rectangleToMerge.size(); i++){
        if(rectangleToMerge[i].tl().y < ymin)
            ymin = rectangleToMerge[i].tl().y;
        if(rectangleToMerge[i].br().y > ymax)
            ymax = rectangleToMerge[i].br().y;
        if(rectangleToMerge[i].tl().x < xmin)
            xmin = rectangleToMerge[i].tl().x;
        if(rectangleToMerge[i].br().x > xmax)
            xmax = rectangleToMerge[i].br().x;
    }

    // If the image is the original one, then no crop!
    if((xmax-xmin)<0 || (ymax-ymin)<0 || (xmax-xmin)>im.cols || (ymax-ymin)>im.rows){
        xmin=0;
        ymin=0;
        xmax=im.cols-2;
        ymax=im.rows-2;
    }
    // We crop the picture to the boundingBox
    Rect myROI(xmin, ymin, xmax-xmin, ymax-ymin);
    Mat croppedImage = inputImage(myROI);

    return croppedImage;
}
