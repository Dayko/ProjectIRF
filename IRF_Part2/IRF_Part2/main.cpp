#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <omp.h>
#include <fstream>
#include <iostream>

#include "features.h"
#include "preprocessing.h"

#define NBICONS 14
#define NBFOLDERS 35//35
#define NBSHEETS 22//22
#define NBROW 7
#define NBCOLUMNS 5
#define NBFEATURES 10
#define NBFEATURESZONING 9
#define NB_BLOCKS_3x3 9
#define BLOCKS_SIDESIZE_3x3 3
#define NB_BLOCKS_2x2 4
#define BLOCKS_SIDESIZE_2x2 2

using namespace cv;
using namespace std;

string inputPath = "../../../outputDonnee/";
string imgFormat = ".png";
string pathToArff = "../arff/testDB.arff";

string reference_Pic_Names[NBICONS] = { "Accident", "Bomb", "Car", "Casualty", "Electricity", "Fire", "FireBrigade", "Flood", "Gas", "Injury", "Paramedics", "Person", "Police", "RoadBlock"};
string featureName[NBFEATURES] = {"RatioBW", "NbLinesBlackPixels", "NbColsBlackPixels", "HoughLines", "HoughCircles", "BoundingBoxNumber", "GravityCenterX", "GravityCenterY", "CannyEdges", "HWRatio"};
string featureNameZoning[NBFEATURESZONING] = {"RatioBW", "NbLinesBlackPixels", "NbColsBlackPixels", "HoughLines", "HoughCircles", "BoundingBoxNumber", "GravityCenterX", "GravityCenterY", "CannyEdges"};

Mat preprocessing(Mat im);
int computeFeatures(Mat im, float tab[], bool divided);
int computeFeaturesZoning(Mat im, float tab[], bool divided);

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
		for (int x = 0; x < BLOCKS_SIDESIZE_3x3; x++)
			for (int y = 0; y < BLOCKS_SIDESIZE_3x3; y++)
                for(int f=0; f<NBFEATURESZONING; f++){
					file << "@ATTRIBUTE " << featureNameZoning[f] << "_" << BLOCKS_SIDESIZE_3x3 << "x" << BLOCKS_SIDESIZE_3x3 << "_" << x << y << " numeric" << endl;
                }
		for (int x = 0; x < BLOCKS_SIDESIZE_2x2; x++)
			for (int y = 0; y < BLOCKS_SIDESIZE_2x2; y++)
				for (int f = 0; f<NBFEATURESZONING; f++){
					file << "@ATTRIBUTE " << featureNameZoning[f] << "_" << BLOCKS_SIDESIZE_2x2 << "x" << BLOCKS_SIDESIZE_2x2 << "_" << x << y << " numeric" << endl;
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
                            Mat im = imread(inputPath + reference_Pic_Names[i] + "_" + scripterString + "_" + pageString + "_" + to_string(l) + "_" + to_string(m) + imgFormat, CV_LOAD_IMAGE_COLOR);
                            if(!im.data){
                                cantopen++;
                            } else {
                                openok++;

                                std::stringstream sstm;
                                sstm << reference_Pic_Names[i];

                                // DO  PRE-PROCESSING
                                Mat impreproc = preprocessing(im);
								//Mat impreproc = im;

                                // COMPUTE FEATURES
								float featureResults[NBFEATURES + NBFEATURESZONING * NB_BLOCKS_3x3 + NBFEATURESZONING * NB_BLOCKS_2x2];
                                int nbResults = computeFeatures(impreproc, featureResults, false);

                                // Divide image in 9 blocks and compute their features (3x3)
								for (int x = 0; x < BLOCKS_SIDESIZE_3x3; x++)
                                {
									for (int y = 0; y < BLOCKS_SIDESIZE_3x3; y++)
                                    {
										int blockWidth = (int)(impreproc.cols / BLOCKS_SIDESIZE_3x3);
										int blockHeight = (int)(impreproc.rows / BLOCKS_SIDESIZE_3x3);
                                        Mat imBlock = impreproc( Rect(x * blockWidth, y * blockHeight, blockWidth, blockHeight) );
                                        nbResults += computeFeaturesZoning(imBlock, featureResults + nbResults, true);
                                    }
                                }
								// Divide image in 4 blocks and compute their features (2x2)
								for (int x = 0; x < BLOCKS_SIDESIZE_2x2; x++)
								{
									for (int y = 0; y < BLOCKS_SIDESIZE_2x2; y++)
									{
										int blockWidth = (int)(impreproc.cols / BLOCKS_SIDESIZE_2x2);
										int blockHeight = (int)(impreproc.rows / BLOCKS_SIDESIZE_2x2);
										Mat imBlock = impreproc(Rect(x * blockWidth, y * blockHeight, blockWidth, blockHeight));
										nbResults += computeFeaturesZoning(imBlock, featureResults + nbResults, true);
									}
								}
                                // ADD VALUE TO THE ARFF FILE
                                for(int rs=0; rs<nbResults; rs++){
                                    sstm << ", " << featureResults[rs];
                                }
                                sstm << endl;

                                #pragma omp critical
                                file << sstm.str();
								cout << "line added icone : " << i << endl;

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
    return preProcess(im);
}

int computeFeatures(Mat im, float tab[], bool divided){
    int size = 0;

    size += featureBW(im, tab + size);
    size += featureNbBlackPixelLinesCols(im, tab + size);
    size += featureHoughLines(im, tab + size);
    size += featureHoughCircles(im, tab + size);
	size += featureBoundingBoxNumber(im, tab + size);
    size += featureGravityCenter(im, tab + size);
    size += featureCannyEdge(im, tab + size);
    size += featureHWRatio(im, tab +size);

    //size += featureHistogram(im, tab + size); // TODO
    return size;
}

int computeFeaturesZoning(Mat im, float tab[], bool divided){
    int size = 0;

    size += featureBW(im, tab + size);
    size += featureNbBlackPixelLinesCols(im, tab + size);
    size += featureHoughLines(im, tab + size);
    size += featureHoughCircles(im, tab + size);
	size += featureBoundingBoxNumber(im, tab + size);
    size += featureGravityCenter(im, tab + size);
    size += featureCannyEdge(im, tab + size);

    //size += featureHistogram(im, tab + size); // TODO
    return size;
}
