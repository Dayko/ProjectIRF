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