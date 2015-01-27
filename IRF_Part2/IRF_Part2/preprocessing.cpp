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

	
    //Dilaating to reduce possible noise
	Mat im_closed;
    int morph_size = 2;
    Mat DilateElement = getStructuringElement(0, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	morphologyEx(im_bounding, im_closed, 3, DilateElement);

	
    //Opening to strenghten the lines again and to fill unwanted gaps
	Mat im_opened;
	Mat OpeningElement = getStructuringElement(0, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	morphologyEx(im_closed, im_opened, 2, OpeningElement);

    //Median filter
    /*for (int i = 1; i < 3; i = i + 2)
    {
        medianBlur(im_out, im_out, i);
    }*/

	/*int i = rand() % 15000;
	string path2 = to_string(i);// +"-" + to_string(rectangleToMerge.size());
	imwrite("../output2/" + path2 + "acrop.png", im_bounding);
	imwrite("../output2/" + path2 + "bclosed.png", im_closed);
	imwrite("../output2/" + path2 + "copened.png", im_opened);*/
	
	return im_opened;
}
