#include "features.h"



int featureHistogram(Mat im, int tab[])
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
	cout << tab[0] << endl;
    
	return 1;
}


int featureBW(Mat im, int tab[])
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
	tab[0] = (int) (countBlack / countTotal * 100);

	return 1;
}


int featureNbHorizontal(Mat im, int tab[])
{
	int limitMin = 250;
	int limitMax = 255;
	Mat imThr;

    threshold(im, imThr, limitMin, limitMax, THRESH_BINARY);
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
		if (lineCountBlack > cols * 0.1)
		{
			nbLinesBlack = nbLinesBlack + 1;
		}
    }
	tab[0] = (int) (nbLinesBlack / nbLinesTotal * 100);
	//cout << tab[0] << endl;

	return 1;
}


int featureNbVertical(Mat im, int tab[])
{
	int limitMin = 250;
	int limitMax = 255;
	Mat imThr;

    threshold(im, imThr, limitMin, limitMax, THRESH_BINARY);
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
		if (colCountBlack > rows * 0.1)
		{
			nbColsBlack = nbColsBlack + 1;
		}
    }
	tab[0] = (int) (nbColsBlack / nbColsTotal * 100);
	//cout << tab[0] << endl;

	return 1;
}

