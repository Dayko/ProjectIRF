#ifndef IMAGETTE_H
#define IMAGETTE_H
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>

#include "cv.h"
#include "highgui.h"
#include "basicMatMethods.h"

using namespace std;
using namespace cv;


enum IMAGETTE_SIZE {
	SMALL_SIZE,
	MEDIUM_SIZE,
	LARGE_SIZE
};

void getAllImagettes(string outputPath, Mat inputImage, string inputPaths[], int correspondantPathsId[], string scripter, string page, string imgFormat);
void writeDescriptionFile(string outputPath, string label, string scripter, string page, int row, int column, IMAGETTE_SIZE size);



#endif // IMAGETTE_H
