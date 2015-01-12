#include "imagette.h"

Mat preProcess(Mat inputImage)
{
	Mat im_out = inputImage;
	//Grayscale
	cvtColor(im_out, im_out, CV_BGR2GRAY);
	//Opening to reduce possible noise (erosion+dilation)
	int morph_size = 1;
	Mat element = getStructuringElement(0, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	morphologyEx(im_out, im_out, 1, element);

	//Closing to strenghten the lines again and to fill unwanted gaps
	morphologyEx(im_out, im_out, 2, element);

	//Median filter
	for (int i = 1; i < 3; i = i + 2)
	{
		medianBlur(im_out, im_out, i);
	}

	//TODO: Center each figure and normalize scaling?

	//Mat threshold_output;
	//vector<vector<Point> > contours;
	//vector<Vec4i> hierarchy;

	//threshold(im_out, threshold_output, 100, 200, CV_THRESH_BINARY_INV);

	//findContours(threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	///// Find the rotated rectangles
	//vector<RotatedRect> minRect(contours.size());
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	minRect[i] = minAreaRect(Mat(contours[i]));
	//}

	///// Draw contours + rotated rects + ellipses
	//Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	//for (int i = 0; i< contours.size(); i++)
	//{
	//	Scalar color = Scalar(0,255,0);
	//	// contour
	//	drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	//	// rotated rectangle

	//	Point2f rect_points[4]; minRect[i].points(rect_points);

	//	for (int j = 0; j < 4; j++)
	//		line(im_out, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
	//}

	return im_out;
}

void getAllImagettes(string outputPath, Mat inputImage, string inputPaths[], int correspondantPathsId[], string scripter, string page, string imgFormat) {
    int cropWidth = 1740;
    int cropHeight = 2450;

    Rect rCrop(550, 680, cropWidth, cropHeight);
    Mat imCrop = inputImage(rCrop);
    //imwrite(outputPath + "_cropped.png", imCrop);

    for (int y = 0; y < NBROW; y++)
    {
        for (int x = 0; x < NBCOLUMN; x++)
        {
            Rect rThumb(x * (cropWidth / NBCOLUMN), y * (cropHeight / NBROW), (cropWidth / NBCOLUMN), (cropHeight / NBROW));
            Mat imThumb = imCrop(rThumb);

            Mat imCanny, gray, edge;
            cvtColor(imThumb, gray, CV_BGR2GRAY);
            Canny(gray, edge, 100, 200, 3);
            edge.convertTo(imCanny, CV_8U);

            // Remove square contour
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;
            findContours(edge, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

            vector<vector<Point> > contours_poly(contours.size());
            Rect boundRect, maxBoundRect;
            for (int i = 0; i < contours.size(); i++)
            {
                approxPolyDP(Mat(contours[i]), contours_poly[i], 3, false);
                boundRect = boundingRect(Mat(contours_poly[i]));
                if (boundRect.size().height > maxBoundRect.size().height)
                    maxBoundRect = boundRect;
                drawContours(imCanny, contours, i, (0, 255, 255), 2, 8, hierarchy, 0, Point());
            }
            imThumb = imThumb(Rect(maxBoundRect.x + 10, maxBoundRect.y + 10, maxBoundRect.width - 20, maxBoundRect.height - 20));

            // Reduce to the bounding box
            /*edge = edge(Rect(maxBoundRect.x + 10, maxBoundRect.y + 10, maxBoundRect.width - 20, maxBoundRect.height - 20));
            imCanny = imCanny(Rect(maxBoundRect.x + 10, maxBoundRect.y + 10, maxBoundRect.width - 20, maxBoundRect.height - 20));

            findContours(edge, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
            vector<vector<Point> > contours2_poly(contours.size());
            maxBoundRect = Rect(0, 0, 0, 0);
            for (int i = 0; i < contours.size(); i++)
            {
                approxPolyDP(Mat(contours[i]), contours2_poly[i], 3, false);
                boundRect = boundingRect(Mat(contours2_poly[i]));
                if (boundRect.size().height > maxBoundRect.size().height)
                    maxBoundRect = boundRect;
            }

            imThumb = imThumb(maxBoundRect);*/

			//Do some pre-processing
			imThumb = preProcess(imThumb);
            // Save the final imagette
            string fullPath = outputPath + inputPaths[correspondantPathsId[y]] + "_" + scripter + "_" + page + "_" + to_string(y) + "_" + to_string(x) + imgFormat ;
            imwrite(fullPath, imThumb);
        }
    }
}


void writeDescriptionFile(string outputPath, string label, string scripter, string page, int row, int column, IMAGETTE_SIZE size) {
    string fullPath = outputPath + label + "_" + scripter + "_" + page + "_" + to_string(row) + "_" + to_string(column) + ".txt";

    ofstream file;
    file.open(fullPath);

    file << "# IRF Project - 2014 - G. Biez, A. Jaury, D. Le Guen, V. Pukari" << endl;
    file << "label " << label << endl;
    file << "form " << scripter << page << endl;
    file << "scripter " << scripter << endl;
    file << "page " << page << endl;
    file << "row " << row << endl;
    file << "column " << column << endl;
    file << "size " << to_string(size) << endl;

    file.close();
}

void getSmallPicRef(Mat src_rot, Mat src_rot_refPic[]){
    cv::Rect myROI(src_rot.cols/16, 3*src_rot.rows/16, src_rot.cols-14*src_rot.cols/16, src_rot.rows-5*src_rot.rows/16);
    splitImage(src_rot(myROI), src_rot_refPic,src_rot(myROI).rows/NBROW);
}

//void getCorrespondanceToRefPic(Mat reference_Pic_RGB[], Mat src_rot_refPic[], int correspondant_Ref_Pic[]){

//    for(int i=0; i<NBROW; i++){
//        double minThreshold_forPic=1;
//        double ThresholdValue;
//        int closestRefPic=0;
//        for(int j=0; j<NBICONREF; j++){
//            // Try to find crosses on the rotated image
//            Point points[1]; //The two crosses coord.
//            ThresholdValue = getPointsFromRefImage(reference_Pic_RGB[j], src_rot_refPic[i], points, 1);
//            //cout << "Threshold for pic " << i << " with " << reference_Pic_Names[i] << " of :" << ThresholdValue << endl;
//            if(minThreshold_forPic>ThresholdValue){
//                minThreshold_forPic=ThresholdValue;
//                closestRefPic = j;
//                cout << i << j << ThresholdValue << endl;
//            }
//        }

//        correspondant_Ref_Pic[i]=closestRefPic;

//    }
//}

void getCorrespondanceToRefPic(Mat reference_Pic_RGB[], Mat src_rot_refPic[], int correspondant_Ref_Pic[]){

    for(int i=0; i<NBROW; i++){
        double thresholdValue=0;
        bool imageFound=false;

        while(thresholdValue < NONDETECTIONLIMIT && !imageFound){
            thresholdValue+=0.01;
            for(int j=0; j<NBICONREF && !imageFound; j++){
                if(isThereMatchs(reference_Pic_RGB[j], src_rot_refPic[i], thresholdValue)){
//                    cout << "image found: " << i << " ; "<< j << " ; "<< thresholdValue << " ; "<< endl;
                    correspondant_Ref_Pic[i]=j;
                    imageFound=true;
                }
            }
        }
        if(!imageFound){
//            cout << "image not found: " << i << endl;
            correspondant_Ref_Pic[i]=NBICONREF;
        }

    }
}
