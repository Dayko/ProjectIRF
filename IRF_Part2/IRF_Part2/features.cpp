#include "features.h"



int featureHistogram(Mat im, float tab[])
{
	
    /// Convert to HSV
	Mat imhsv;
    cvtColor( im, imhsv, COLOR_BGR2HSV );

    int hbins = 30, sbins = 32;
    int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, saturation from 0 to 255
	float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    int channels[] = {0, 1};

    /// Histograms
    MatND hist;
    calcHist( &imhsv, 1, channels, Mat(), hist, 2, histSize, ranges, true, false );
	
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);

	int maxI = 10;
	int maxIntensity = 0.0;
	for( int h = 0; h < hbins; h++ )
        for( int s = 0; s < sbins; s++ )
	{
		float histValue = hist.at<float>(h, s);
		int intensity = cvRound(histValue*255/maxVal);
		if (intensity > maxIntensity)
		{
			maxIntensity = intensity;
			maxI = s;
		}
	}

	tab[0] = maxI;
	//cout << tab[0] << endl;
    
	return 1;
}


int featureBW(Mat im, float tab[])
{
	int limitMin = 250;
	int limitMax = 255;
	Mat imThr;

    threshold(im, imThr, limitMin, limitMax, THRESH_BINARY);
    float countTotal = (float)(im.rows * im.cols);
    float countBlack = 0.0;

    for (int j = 0; j < im.rows; j++)
    {
        for (int i = 0; i < im.cols; i++)
        {
            if (imThr.at<uchar>(j, i) == 0)
                countBlack = countBlack + 1;
        }
    }
	tab[0] = countBlack / countTotal * 100;

	return 1;
}


int featureNbBlackPixelLinesCols(Mat im, float tab[])
{
	float thresholdValue = 0.05;

	int limitMin = 250;
	int limitMax = 255;
	Mat imThr;

    threshold(im, imThr, limitMin, limitMax, THRESH_BINARY);

	// Count black pixel on each lines
	float cols = (float)im.cols;
	float nbLinesTotal = (float)im.rows;
	float nbLinesBlack = 0;

	for (int j = 0; j < im.rows; j++)
	{
		int lineCountBlack = 0;
		for (int i = 0; i < im.cols; i++)
		{
			if (imThr.at<uchar>(j, i) == 0)
				lineCountBlack = lineCountBlack + 1;
		}
		if (lineCountBlack > cols * thresholdValue)
		{
			nbLinesBlack = nbLinesBlack + 1;
		}
	}
	tab[0] = nbLinesBlack / nbLinesTotal;

	// Count black pixel on each columns
	float rows = (float)im.rows;
	float nbColsTotal = (float)im.cols;
	float nbColsBlack = 0;

    for (int i = 0; i < im.cols; i++)
    {
		int colCountBlack = 0;
		for (int j = 0; j < im.rows; j++)
        {
            if (imThr.at<uchar>(j, i) == 0)
                colCountBlack = colCountBlack + 1;
        }
		if (colCountBlack > rows * thresholdValue)
		{
			nbColsBlack = nbColsBlack + 1;
		}
    }
	tab[1] = nbColsBlack / nbColsTotal;
	//cout << tab[0] << endl;

	return 2;
}


int featureHoughLines(Mat im, float tab[])
{
	double lineMinLength = 40;

	Mat dst;
	Canny(im, dst, 50, 200, 3);

	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, lineMinLength);

	tab[0] = lines.size();
	//cout << tab[0] << endl;

	return 1;
}

int featureBoundingRatio(Mat im, float tab[]){
	// Convert Image to gray
	Mat im_gray;

	cvtColor(im, im_gray, CV_BGR2GRAY);

	// blur the image a little
	blur(im_gray, im_gray, Size(3, 3));

	/// Detect edges using Threshold
	Mat threshold_output;
	int thresh = 253;
	threshold(im_gray, threshold_output, thresh, 255, THRESH_BINARY);

	// Finds contours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}
	/// Return in the table of the number of bounding boxes
	vector<vector<Point> > contoursToMerge;
	cout << "Size of image : Height - " << im.size().height << " -- Width - " << im.size().width << endl;
	for (int i = 0; i < boundRect.size(); i++)
	{
		/*Scalar color = Scalar(255, 0, 255);
		drawContours(im, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(im, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);*/

		//test of the Rectangle :
		// If it's approximatly equal to the dimension, in height or width, of the whole image, then it's probably the box of the whole image
		// or it's a wrong box wich result of a remaining border of an imagette.
		// We also discard too small rectangles
		cout << " Height : " << boundRect[i].height << " -- Width : " << boundRect[i].width << endl;
		cout << " Area : " << boundRect[i].area << endl;
		/*if (((boundRect[i].height - im.size().height) < im.size().height*0.05)
			&& ((boundRect[i].width - im.size().width) < im.size().width*0.05)
			&& boundRect)*/

	}

	/*int i = rand() % 500;
	imwrite("../output/" + to_string(i) +".png", im);*/
}

int featureHoughCircles(Mat im, float tab[])
{
	double lineMinLength = 40;

	Mat dst;
	cvtColor(im, dst, CV_BGR2GRAY);

	// smooth it, otherwise a lot of false circles may be detected
	GaussianBlur(dst, dst, Size(9, 9), 2, 2);
	vector<Vec3f> circles;
	HoughCircles(dst, circles, CV_HOUGH_GRADIENT, 2, dst.rows/4, 100, 20);

	tab[0] = circles.size();
	//cout << tab[0] << endl;

	return 1;
}