#include "rotate_picture.h"

double getFineAngle(Mat src, Mat ref){
    double angle = 1;
    double angle_ref = 1;

    // Try to find crosses on the image
    Point points[2]; //The two crosses coord.
    if(getPointsFromRefImage(ref,  src, points, 2) > NONDETECTIONLIMIT){
        return 0; // Return an error (real value never equals to 0)
    }

    int delta_x = abs(points[0].x-points[1].x);
    int delta_y = abs(points[0].y-points[1].y);

    if(delta_x==0 || delta_y==0){
        return 0; // Return an error (real value never equals to 0)
    }

    angle = atan((double)delta_y/(double)delta_x);

//    cout << "Computing a more precise angle... " << endl;
//    cout << "DeltaX: " << delta_x << " ;  DeltaY: " << delta_y << endl;
//    cout << "angle fine: " << angle-angle_ref << endl;

    if(angle==angle_ref){
        angle_ref+=0.00001; // To have a different value than 0
    }

    return angle-angle_ref;
}

int getMajorAngle(Mat src, Mat ref){
    bool pointsfound = false;
    int angle;

    //Gets reference icons, converts in HSV and computes histograms
    int inc=1;

    for(angle=0; !pointsfound ; angle+=inc){
        if(angle>=5) {
            angle = -1; //Compute negative values
            inc = -inc;
        }

        Mat src_ang = getRotatedImage(src, angle);

        // Try to find crosses on the rotated image
        Point points[2]; //The two crosses coord.
        if(getPointsFromRefImage(ref,  src_ang, points, 2) < NONDETECTIONLIMIT){
            pointsfound = true;
        }
        if(angle<-5){
            return -10;
        }
    }
    return angle-inc;
}

Mat getRotatedImage(Mat src, double angle){

    if(angle==0){
        return src;
    }

    // Rotate according to correction_angle variable. Note: must be opposite angle
    Point2f center((src.cols/2.0F, src.rows/2.0F));
    Mat rot_mat = getRotationMatrix2D(center, -angle, 1.0);

    Mat rotatedImage;
    warpAffine(src, rotatedImage, rot_mat, src.size(),1,0,Scalar(255,255,255));

    return rotatedImage;
}





bool getImageCorrectlyRotated(string path, Mat refCrosses, Mat *src_rotated){
    Mat src = loadImage(path);

    int angle = getMajorAngle(getSmallerImage(src,4), getSmallerImage(refCrosses,4));

    if(angle==-10){
        cout << "impossible to find the crosses" << endl;
        return false;
    }

//    cout << "Getting major angle..." << endl;
//    cout << "angle = " << angle << endl;

    Mat src_rot = getRotatedImage(src, (double) angle);

    double anglefine = getFineAngle(getSmallerImage(src_rot,4), getSmallerImage(refCrosses,4));// Get a more precise rotation angle
    Mat src_rot_fine = getRotatedImage(src, angle+anglefine);

    *src_rotated=src_rot_fine;

    return true;
}
