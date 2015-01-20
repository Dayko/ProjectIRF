#include "cv.h"
#include "highgui.h"
#include "omp.h"
#include "features.h"
#include <fstream>
#include <iostream>


#define NBICONS 14
#define NBFOLDERS 3//35
#define NBSHEETS 22//22
#define NBROW 7
#define NBCOLUMNS 5
#define NBFEATURES 10
#define NBBLOCKS 9
#define BLOCKSSIDESIZE 3

using namespace cv;
using namespace std;

string inputPath = "../../output/";
string imgFormat = ".png";
string pathToArff = "../arff/test4.arff";

string reference_Pic_Names[NBICONS] = { "Accident", "Bomb", "Car", "Casualty", "Electricity", "Fire", "FireBrigade", "Flood", "Gas", "Injury", "Paramedics", "Person", "Police", "RoadBlock"};
string featureName[NBFEATURES] = {"RatioBW", "NbLinesBlackPixels", "NbColsBlackPixels", "HoughLines", "HoughCircles", "BoundingBoxNumber", "BoundingBoxRatio", "GravityCenterX", "GravityCenterY", "CannyEdges"};

Mat preprocessing(Mat im);
int computeFeatures(Mat im, float tab[]);

int main(int argc, char *argv[])
{

    //Open ARFF File
    ofstream file;
    file.open(pathToArff, ios::out | ios::ate );

    if(file) {  // If file exists

        //Initialize ARFF File
        file << "@RELATION imagette" << endl << endl;
        file << "@ATTRIBUTE label {Accident, Bomb, Car, Casualty, Electricity, Fire, FireBrigade, Flood, Gas, Injury, Paramedics, Person, Police, RoadBlock}" << endl;

        for(int f=0; f<NBFEATURES; f++){
            file << "@ATTRIBUTE " << featureName[f] << "_Global" << " numeric" << endl;
        }
		for (int x = 0; x < BLOCKSSIDESIZE; x++)
			for (int y = 0; y < BLOCKSSIDESIZE; y++)
				for(int f=0; f<NBFEATURES; f++){
					file << "@ATTRIBUTE " << featureName[f] << "_" << x << y << " numeric" << endl;
				}

        file << endl;
        file << "@DATA" << endl;

#pragma omp parallel for shared(file)
        for(int i=0; i<NBICONS; i++){
            int cantopen=0, openok=0;

            for(int j=0; j<NBFOLDERS; j++){

                string scripterString = to_string(j);
                int scripterStringLength = scripterString.length();
                for (int n = 0; n < 3 - scripterStringLength; n++) {
                    scripterString = "0" + scripterString;
                }

                for(int k=0; k<NBSHEETS; k++){
                    string pageString = to_string(k);
                    int pageStringLength = pageString.length();
                    for (int p = 0; p < 2 - pageStringLength; p++) {
                        pageString = "0" + pageString;
                    }

                    for(int l=0; l<NBROW; l++){
                        for(int m=0; m<NBCOLUMNS; m++){

                            //OPEN IMAGE FILE
                            Mat im = imread(inputPath + reference_Pic_Names[i] + "_" + scripterString + "_" + pageString + "_" + to_string(l) + "_" + to_string(m) + imgFormat);
                            if(im.data == NULL){
                                cantopen++;
                            } else {
                                openok++;
								
                                std::stringstream sstm;
                                sstm << reference_Pic_Names[i];

                                // DO  PRE-PROCESSING
                                Mat impreproc = preprocessing(im);

                                // COMPUTE FEATURES
								float featureResults[NBFEATURES + NBFEATURES * NBBLOCKS];
                                int nbResults = computeFeatures(im, featureResults, false);

								// Divide image in 9 blocks and compute their features (3x3)
								for (int x = 0; x < BLOCKSSIDESIZE; x++)
								{
									for (int y = 0; y < BLOCKSSIDESIZE; y++)
									{
										int blockWidth = (int)(im.cols / BLOCKSSIDESIZE);
										int blockHeight = (int)(im.rows / BLOCKSSIDESIZE);
										Mat imBlock = im( Rect(x * blockWidth, y * blockHeight, blockWidth, blockHeight) );
										nbResults += computeFeatures(imBlock, featureResults + nbResults, true);
									}
								}
                                // ADD VALUE TO THE ARFF FILE
                                for(int rs=0; rs<nbResults; rs++){
                                    sstm << ", " << featureResults[rs];
                                }
                                sstm << endl;

                            #pragma omp critical
                                file << sstm.str();


							}
						}
					}
				}
			}
			cout << "Computing " << reference_Pic_Names[i] << "... There are : " << cantopen << " impossible to open and " << openok << " images open" << endl;
		}

		//CLOSE ARFF FILE
		file.close();
	} else {
		cerr << "Error creating file!" << endl;
	}

	return 0;
}


Mat preprocessing(Mat im){
    return im;
}

int computeFeatures(Mat im, float tab[], bool divided){
	int size = 0;

	size += featureBW(im, tab + size);
	size += featureNbBlackPixelLinesCols(im, tab + size);
	size += featureHoughLines(im, tab + size);
	size += featureHoughCircles(im, tab + size);
	size += featureBoundingRatio(im, tab + size, divided);
	size += featureGravityCenter(im, tab + size);
	size += featureCannyEdge(im, tab + size);
	//size += featureHistogram(im, tab + size); // TODO
    return size;
}
