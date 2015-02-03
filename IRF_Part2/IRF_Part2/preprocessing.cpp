#include "preprocessing.h"
#include "normalization.h"


Mat preProcess(Mat inputImage)
{
	// Convert Image to gray
	Mat im_gray;

	cvtColor(inputImage, im_gray, CV_BGR2GRAY);
		
	// Detect edges using Threshold
	Mat threshold_output;
	int thresh = 245;
	threshold(im_gray, threshold_output, thresh, 255, THRESH_BINARY);

	//eroding to strenghten the lines again and to fill unwanted gaps
	Mat im_eroded;
	erode(threshold_output, im_eroded, Mat(), Point(-1, -1), 1, 1, 1);

	// Before cropping : add some blank border around the image to avoid some errors regarding bounding boxes after
	Mat im_to_crop;
	im_eroded.copyTo(im_to_crop);
	Scalar color = Scalar(255, 255, 255);
	copyMakeBorder(im_eroded, im_to_crop, (int)(0.03*im_eroded.rows), (int)(0.03*im_eroded.rows), (int)(0.03*im_eroded.cols), (int)(0.03*im_eroded.cols),BORDER_CONSTANT,color);
	
	// Crop the symbol
	Mat im_bounding = BoundingRatio(im_to_crop);

	return im_bounding;
}
