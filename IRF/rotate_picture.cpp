#include "rotate_picture.h"

double getFineAngle(Mat src){
    double angle;
    double angle_ref = 1;

    //Gets reference icons, converts in HSV and computes histograms
    Mat ref = loadImage("../reference/Cross-small.png");

    Point points[2];
    getPointsFromRefImage(getSmallerImage(ref,2), getSmallerImage(src,2), points);

    int delta_x = abs(points[0].x-points[1].x);
    int delta_y = abs(points[0].y-points[1].y);

    angle = atan((double)delta_y/(double)delta_x);

    cout << "X: " << delta_x << " ;  Y: " << delta_y << endl;
    cout << "angle fine: " << angle-angle_ref << endl;

    return angle-angle_ref;
}

int findCorrectAngle(Mat src){
    double minThreshold = 1;
    int angle;

    //Gets reference icons, converts in HSV and computes histograms
    Mat ref = loadImage("../reference/Cross-small.png");

    int inc=1;

    for(angle=0; minThreshold>0.1 && inc>-5; angle+=inc){
        Mat src_ang = getRotatedImage(src, angle);
        minThreshold = getMinThresholdValue(getSmallerImage(ref,2), getSmallerImage(src_ang,2), 2);
        cout << "Threshold of :" << minThreshold<< " for angle "<< angle << endl;
        if(angle>=5) {angle = 0; inc = -inc;}
    }
    return angle-inc;
}

Mat getRotatedImagedouble(Mat src, double angle){
    // Rotate according to correction_angle variable. Note: must be opposite angle
    Point2f center((src.cols/2.0F, src.rows/2.0F));
    Mat rot_mat = getRotationMatrix2D(center, -angle, 1.0);

    Mat rotated;
    warpAffine(src, rotated, rot_mat, src.size(),1,0,Scalar(255,255,255));

    return rotated;
}

Mat getRotatedImage(Mat src, int angle){
    return getRotatedImagedouble(src, (double) angle);
}

double getPointsFromRefImage(Mat ref, Mat src, Point points[]){
    Mat gref, gsrc;
    int numberMatches = 0;
    double thresholdMinValue = 0.05;

    // Convert both images to grayscale. Just to be sure
    cvtColor(ref, gref, CV_BGR2GRAY);
    cvtColor(src, gsrc, CV_BGR2GRAY);

    // Use cv::matchTemplate to find the matches
    Mat res(src.rows-ref.rows+1, src.cols-ref.cols+1, CV_32FC1);
    matchTemplate(gsrc, gref, res, CV_TM_CCOEFF_NORMED);
    threshold(res, res, 0.72, 1., CV_THRESH_TOZERO);

    while (true)
    {
        double minval, maxval;
        Point minloc, maxloc;
        minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

        if (maxval >= thresholdMinValue){
            bool differentPicture = true;

            for(int i=0; i<numberMatches; i++){
                if(euclideanDist(points[i], maxloc)<5.0){
                    differentPicture=false;
                }
            }


            if(differentPicture){
                points[numberMatches]=maxloc;
                cout << "pount found" << maxloc.x << " ; " << maxloc.y << endl;
                numberMatches++;
            }
            floodFill(res, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));
        }
        else
            break;
    }
}

double getMinThresholdValue(Mat ref, Mat src, int numberOfMatches){
    Mat gref, gsrc;
    int numberMatches = 0;
    double thresholdMinValue = 0.0;

    // Convert both images to grayscale. Just to be sure
    cvtColor(ref, gref, CV_BGR2GRAY);
    cvtColor(src, gsrc, CV_BGR2GRAY);



    //     clock_t tStart = clock();
    // Use cv::matchTemplate to find the matches
    Mat res(src.rows-ref.rows+1, src.cols-ref.cols+1, CV_32FC1);
    matchTemplate(gsrc, gref, res, CV_TM_CCOEFF_NORMED);
    threshold(res, res, 0.72, 1., CV_THRESH_TOZERO);
    //   printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    while(numberMatches<numberOfMatches && thresholdMinValue<NONDETECTIONLIMIT){
        thresholdMinValue+=0.001;
        //        printf("thresholdMinValue %lf\n",thresholdMinValue);
        numberMatches=0;
        vector<Point> tabPoint(numberOfMatches);
        while (true)
        {
            double minval, maxval;
            Point minloc, maxloc;
            minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

            if (maxval >= thresholdMinValue){
                bool differentPicture = true;

                for(int i=0; i<numberMatches; i++){
                    if(euclideanDist(tabPoint[i], maxloc)<5.0){
                        differentPicture=false;
                    }
                }

                if(differentPicture){
                    tabPoint[numberMatches]=maxloc;
                    numberMatches++;
                    //                    printf("numberMatches %d: %d ; %d\n",numberMatches, maxloc.x, maxloc.y);
                }
                floodFill(res, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));
                //                printf("NotGood %d: %d ; %d\n",numberMatches, maxloc.x, maxloc.y);
            }
            else
                break;
        }
        //        if(numberMatches!=0)
        //         cout << "Number of Matches " << numberMatches << " / " << numberOfMatches << " " << thresholdMinValue << endl;
    }
    //imshow("azazazaz",res);

    return thresholdMinValue;
}

float euclideanDist(Point& p, Point& q) {
    Point diff = p - q;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}


Mat getImageCorrectlyRotated(string path){
    Mat src = loadImage(path);

    int angle = findCorrectAngle(src);
    cout << "angle" << angle << endl;
    Mat src_rot = getRotatedImage(src, angle);

    return src_rot;
}

void getSmallPicRef(Mat src_rot, Mat src_rot_refPic[]){
    cv::Rect myROI(src_rot.cols/16, 3*src_rot.rows/16, src_rot.cols-14*src_rot.cols/16, src_rot.rows-5*src_rot.rows/16);
    splitImage(src_rot(myROI), src_rot_refPic,src_rot(myROI).rows/NBROW);
}

void getCorrespondanceToRefPic(Mat reference_Pic_RGB[], Mat src_rot_refPic[], int correspondant_Ref_Pic[]){

    for(int i=0; i<NBROW; i++){
        double minThreshold_forPic=1;
        double ThresholdValue;
        int closestRefPic=0;
        for(int j=0; j<NBICONREF; j++){
            ThresholdValue = getMinThresholdValue(reference_Pic_RGB[j], src_rot_refPic[i], 1);
            //cout << "Threshold for pic " << i << " with " << reference_Pic_Names[i] << " of :" << ThresholdValue << endl;
            if(minThreshold_forPic>ThresholdValue){
                minThreshold_forPic=ThresholdValue;
                closestRefPic = j;
            }
        }
        correspondant_Ref_Pic[i]=closestRefPic;

    }
}
