#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main() 
{
	// Vector to store coordinates
	vector<int> xVal,yVal;

    Mat ref = imread("reference/Cross-small.png");
    // Mat src_org = imread("all-scans/w000-scans/00002.png");
	Mat src_org = imread("test/s01_0001.png");

	// Creates new image by rotating original 5 degrees. Just to speed up testing
	Point2f src_center((src_org.cols/2.0F, src_org.rows/2.0F));
	Mat rot_mat = getRotationMatrix2D(src_center, 5, 1.0);

	Mat src;

	// Fix this. Doesn't center the image correctly.
	warpAffine(src_org, src, rot_mat, src_org.size(),1,0,Scalar(255,255,255));

    if (ref.empty() || src.empty())
        return -1;
	
	int ref_points = 0;
	int angle = 0;
	int correction_angle = 0;


	// To speed up the process, reduce both images by 4
	int reduction = 4;
	Size tailleReduite(src.cols/reduction, src.rows/reduction);
	Mat imreduite = Mat(tailleReduite,CV_8UC3);
	resize(src,imreduite,tailleReduite);

	Size tailleReduite2(ref.cols/reduction, ref.rows/reduction);
	Mat ref_reduite = Mat(tailleReduite2,CV_8UC3);
	resize(ref,ref_reduite,tailleReduite2);

	//imshow("orginal", imreduite);

	// Direction variable to implement angles from 15 degrees to -15 degrees
	int direction = 1;
	int vectSize = xVal.size();

	// Reapeat until atleast two points are found
	printf("Processing: Finding reference points\n");
	while( vectSize < 2)
	{
		// Check the angle, change direction when at the end
		if(angle == 15 && direction == 1)
		{
			direction = -1;
			angle = 0;
		}
		// Break when done
		else if(angle == -15 && direction == -1)
		{
			break;
		}
		angle = angle + direction;
		ref_points = 0;

		// Rotate to get new reference cross
		Point2f src_center(ref_reduite.cols/2.0F, ref_reduite.rows/2.0F);
		Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);

		Mat ref_new;

		ref_new.create(ref_reduite.size().width, ref_reduite.size().height, CV_32FC1);
		warpAffine(ref_reduite, ref_new, rot_mat, ref_new.size(), 1, 0, Scalar(255,255,255));

		Mat gref, gsrc;
	
		// Convert both images to grayscale. Just to be sure
		cvtColor(ref_new, gref, CV_BGR2GRAY);
		cvtColor(imreduite, gsrc, CV_BGR2GRAY);

		// Use cv::matchTemplate to find the matches
		Mat res(imreduite.rows-ref_reduite.rows+1, imreduite.cols-ref_reduite.cols+1, CV_32FC1);
		matchTemplate(gsrc, gref, res, CV_TM_CCOEFF_NORMED);
		threshold(res, res, 0.8, 1., CV_THRESH_TOZERO);

		while (true) 
		{
			double minval, maxval, threshold = 0.001;
			Point minloc, maxloc;
			minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

			if (maxval >= threshold)
			{
				rectangle(
					imreduite, 
					maxloc, 
					Point(maxloc.x + ref_reduite.cols, maxloc.y + ref_reduite.rows), 
					CV_RGB(0,255,0), 2
				);
				// Store angle and correct coordinates
				correction_angle = angle + 1;
				xVal.push_back((int)maxloc.x + ref_reduite.cols/2.);
				yVal.push_back((int)maxloc.y + ref_reduite.rows/2.);
				floodFill(res, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));
				ref_points++;
			}
			else
				break;
		}
		//printf("ref_points: %i, %i\n", ref_points, angle);
	}

	// Rotate according to correction_angle variable. Note: must be opposite angle
	Point2f center((imreduite.cols/2.0F, imreduite.rows/2.0F));
	rot_mat = getRotationMatrix2D(center, -correction_angle, 1.0);

	Mat rotated;
	warpAffine(imreduite, rotated, rot_mat, src_org.size(),1,0,Scalar(255,255,255));

	// To better display of the result image, reduce image size again by 2
	reduction = 2;
	Size tailleReduite3(rotated.cols/reduction, rotated.rows/reduction);
	Mat imreduite2 = Mat(tailleReduite3,CV_8UC3);
	resize(rotated,imreduite2,tailleReduite3);
	imshow("rotated", imreduite2);

    waitKey();
    return 0;
}