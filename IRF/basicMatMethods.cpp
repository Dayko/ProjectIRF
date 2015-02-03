#include "basicMatMethods.h"

Mat loadImage(string imName){
    Mat im = imread(imName);
    if(im.data == NULL){
        cerr << "Image not found: "<< imName << endl;
        exit(0);
    }
    return im;
}

Mat getSmallerImage(Mat inputImage, int sizeReduction){
    if(sizeReduction==0){
        return inputImage;
    }
    Size tailleReduite(inputImage.cols/sizeReduction, inputImage.rows/sizeReduction);
    Mat imreduite = Mat(tailleReduite,CV_8UC3);

    resize(inputImage,imreduite,tailleReduite);
    return imreduite;
}

void loadReferenceImages(string inputPaths[], Mat outputTMat[], int nb){
    for(int i=0; i<nb;i++){
        outputTMat[i] = loadImage("../reference/"+inputPaths[i]+"2.png");
    }
}

void splitImage(Mat src, Mat output[], int height){
    for(int i=0; i<7; i++){
        if(i*height < src.rows){
            cv::Rect myROI(0, i*height, src.cols-1, height);
            output[i]=src(myROI);
        }
    }
}

double getPointsFromRefImage(Mat ref, Mat src, Point points[], int MaxNumberOfMatches){
    Mat gref, gsrc;
    int numberMatches = 0;
    double thresholdMinValue = 0.0;

    double minval, maxval;
    Point minloc, maxloc;

    // Convert both images to grayscale. Just to be sure
    cvtColor(ref, gref, CV_BGR2GRAY);
    cvtColor(src, gsrc, CV_BGR2GRAY);

    // Use cv::matchTemplate to find the matches
    Mat res(src.rows-ref.rows+1, src.cols-ref.cols+1, CV_32FC1);
    matchTemplate(gsrc, gref, res, CV_TM_CCOEFF_NORMED);
    threshold(res, res, 0.72, 1., CV_THRESH_TOZERO);

    while (numberMatches<MaxNumberOfMatches && thresholdMinValue<NONDETECTIONLIMIT)
    {
        thresholdMinValue+=0.001;
        numberMatches=0;

        while (numberMatches<MaxNumberOfMatches)
        {
            minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

            if (maxval >= thresholdMinValue){

                // Check if the found point is too close to another one ----
                bool differentPicture = true;

                for(int i=0; i<numberMatches; i++){
                    if(euclideanDist(points[i], maxloc)<5.0){
                        differentPicture=false;
                    }
                }

                // If different ... we add the point to the list of found points
                if(differentPicture){
                    points[numberMatches]=maxloc;
                   //cout << "point found" << maxloc.x << " ; " << maxloc.y << endl;
                    numberMatches++;
                }

                floodFill(res, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.)); //
            }
            else
                break;
        }
    }
    return thresholdMinValue;
}

bool isThereMatchs(Mat ref, Mat src, double thresholdMinValue){
    Mat gref, gsrc;

    double minval, maxval;
    Point minloc, maxloc;

    // Convert both images to grayscale. Just to be sure
    cvtColor(ref, gref, CV_BGR2GRAY);
    cvtColor(src, gsrc, CV_BGR2GRAY);

    // Use cv::matchTemplate to find the matches
    Mat res(src.rows-ref.rows+1, src.cols-ref.cols+1, CV_32FC1);
    matchTemplate(gsrc, gref, res, CV_TM_CCOEFF_NORMED);
    threshold(res, res, 0.72, 1., CV_THRESH_TOZERO);


    minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

    return (maxval >= thresholdMinValue);
}


float euclideanDist(Point& p, Point& q) {
    Point diff = p - q;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}
