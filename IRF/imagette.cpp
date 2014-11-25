#include "imagette.h"


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