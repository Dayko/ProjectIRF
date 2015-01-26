#include "features.h"



int featureHistogram(Mat im, float tab[])
{

	/// Convert to HSV
	Mat imhsv;
	cvtColor(im, imhsv, COLOR_BGR2HSV);

	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	// hue varies from 0 to 179, saturation from 0 to 255
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	int channels[] = { 0, 1 };

	/// Histograms
	MatND hist;
	calcHist(&imhsv, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);

	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);

	int maxI = 10;
	int maxIntensity = 0.0;
	for (int h = 0; h < hbins; h++)
		for (int s = 0; s < sbins; s++)
		{
			float histValue = hist.at<float>(h, s);
			int intensity = cvRound(histValue * 255 / maxVal);
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


int featureHoughCircles(Mat im, float tab[])
{
	double lineMinLength = 40;

	// smooth it, otherwise a lot of false circles may be detected
    GaussianBlur(im, im, Size(9, 9), 2, 2);
	vector<Vec3f> circles;
    HoughCircles(im, circles, CV_HOUGH_GRADIENT, 2, im.rows / 4, 100, 20);

	tab[0] = circles.size();
	//cout << tab[0] << endl;

	return 1;
}


int featureBoundingBoxNumber(Mat im, float tab[]){
	int areaMax = (im.size().height * im.size().width)*(1 - 0.04);
	int areaMin = areaMax / 40;

	// blur the image a little
    blur(im, im, Size(3, 3));

	/// Detect edges using Threshold
	Mat threshold_output;
	int thresh = 253;
    threshold(im, threshold_output, thresh, 255, THRESH_BINARY);

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

	vector<Rect> rectangleToMerge;
	for (int i = 0; i < boundRect.size(); i++)
	{
		//test of the Rectangle :
		// If it's approximatly equal to the dimension, in height or width, of the whole image, then it's probably the box of the whole image
		// or it's a wrong box wich result of a remaining border line of an imagette. We also check it if the ratio of the box is anormal (too small)
		// We also discard too small rectangles and too big (another check for the whole image bouding box)
		if (!((im.size().height - boundRect[i].height) < im.size().height*0.02)
				&& !((im.size().width - boundRect[i].width) < im.size().width*0.02)
				&& boundRect[i].area() < areaMax && boundRect[i].area() > areaMin
				&& !(((float)(boundRect[i].height) / (float)(boundRect[i].width)) < 0.08 || ((float)(boundRect[i].width) / (float)(boundRect[i].height)) < 0.08)){
			rectangleToMerge.push_back(boundRect[i]);
		}
	}

	/* Impression de des bounding boxes que l'on considère valides */
	/*for (int i = 0; i < rectangleToMerge.size(); i++){
	Scalar color = Scalar(0, 255, 0);
	rectangle(im, rectangleToMerge[i].tl(), rectangleToMerge[i].br(), color, 2, 8, 0);
	}*/
	
	tab[0] = rectangleToMerge.size();

	//Enregistrement de l'image pour observer les résultats
	/*int i = rand() % 1500;
	string path2 = to_string(i);// +"-" + to_string(rectangleToMerge.size());
	imwrite("../output2/" + path2 + ".png", im);*/
	return 1;
}


int featureGravityCenter(Mat im, float tab[])
{
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SurfFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints_1;

    detector.detect(im, keypoints_1);

	//-- Draw keypoints
	//Mat img_keypoints_1;
	//drawKeypoints(input_image, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//-- Show detected (drawn) keypoints
	//imshow("Keypoints 1", img_keypoints_1);
	//waitKey(0);

	Point2f cen(0, 0);
	for (size_t i = 0; i < keypoints_1.size(); i++)
	{
		cen.x += keypoints_1[i].pt.x;
		cen.y += keypoints_1[i].pt.y;
	}
	if (keypoints_1.size() != 0)
	{
		cen.x /= keypoints_1.size();
		cen.y /= keypoints_1.size();
		tab[0] = cen.x;
		tab[1] = cen.y;
	}
	else
	{
		tab[0] = 0;
		tab[1] = 0;
	}
	return 2;
}


int featureCannyEdge(Mat im, float tab[])
{
	Mat edges;
	Canny(im, edges, 100, 100 * 3, 3); // Try with another threshold ?

	vector<std::vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	findContours(edges, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	// Number of edge pixels
	tab[0] = contours.size();

	return 1;
}

int featureHWRatio(Mat im, float tab[])
{
    tab[0] = (float)im.rows/(float)im.cols;
    return 1;
}
