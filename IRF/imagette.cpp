#include "imagette.h"

void getAllImagettes(string outputPath, Mat inputImage, string inputPaths[], int correspondantPathsId[], string scripter, string page, string imgFormat) {
	
	//Get the top right cross
	Rect rTopCrop(0, 0, inputImage.size().width, 570);
	Mat imCropTop = inputImage(rTopCrop);
	// Get cross reference image ----------------------------
	Mat refCrosses = loadImage("../reference/Cross-small.png");
	Point cross[1];
	if (getPointsFromRefImage(refCrosses, imCropTop, cross, 1) > NONDETECTIONLIMIT){
		cout << "Croix en haut à droite non trouvée. Ce message n'apparaîtra jamais" << endl;
	}

	int cropWidth = 1740;
	int cropHeight = 2450;
	//Crop the imagettes zones, taking into account the top right cross
	Rect rCrop((cross[0].x-1587), (cross[0].y+270), cropWidth, cropHeight);
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
                    cout << "image found: " << i << " ; "<< j << " ; "<< thresholdValue << " ; "<< endl;
                    correspondant_Ref_Pic[i]=j;
                    imageFound=true;
                }
            }
        }
        if(!imageFound){
            cout << "image not found: " << i << endl;
            correspondant_Ref_Pic[i]=NBICONREF;
        }

    }
}
