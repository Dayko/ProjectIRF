#ifndef IMAGETTE_H
#define IMAGETTE_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


enum IMAGETTE_SIZE {
	SMALL_SIZE,
	MEDIUM_SIZE,
	LARGE_SIZE
};


void writeImagetteFile(string label, string scripter, string page, int row, int column, string imgFormat);
void writeDescriptionFile(string label, string scripter, string page, int row, int column, IMAGETTE_SIZE size);



#endif // IMAGETTE_H
