#include "preprocessing.h"
#include "normalization.h"


Mat preProcess(Mat inputImage)
{
	// Convert Image to gray
	Mat im_gray;

	cvtColor(inputImage, im_gray, CV_BGR2GRAY);
	
	// blur the image a little
	Mat im_blur;
	blur(im_gray, im_blur, Size(3, 3));


	
	// Detect edges using Threshold
	Mat threshold_output;
	int thresh = 253;
	threshold(im_blur, threshold_output, thresh, 255, THRESH_BINARY);

	Mat im_bounding = BoundingRatio(threshold_output);

	/*
    //Opening to reduce possible noise (erosion+dilation)
	Mat im_out;
    int morph_size = 1;
    Mat element = getStructuringElement(0, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	morphologyEx(im_blur, im_out, 1, element);

    //Closing to strenghten the lines again and to fill unwanted gaps
    morphologyEx(im_out, im_out, 2, element);

    //Median filter
    for (int i = 1; i < 3; i = i + 2)
    {
        medianBlur(im_out, im_out, i);
    }*/

	/*
	int i = rand() % 15000;
	string path2 = to_string(i);// +"-" + to_string(rectangleToMerge.size());
	imwrite("../output2/" + path2 + ".png", threshold_output);
	imwrite("../output2/" + path2 + "S.png", im_bounding);
	*/
	return im_bounding;
}
