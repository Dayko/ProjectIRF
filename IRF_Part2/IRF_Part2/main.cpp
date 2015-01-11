#include "cv.h"
#include "highgui.h"
#include "omp.h"
#include <fstream>
#include <iostream>


#define NBICONS 14
#define NBFOLDERS 35
#define NBSHEETS 22
#define NBROW 7
#define NBCOLUMNS 5
#define NBFEATURES 9

using namespace cv;
using namespace std;

string inputPath = "../output/";
string imgFormat = ".png";
string pathToArff = "../arff/test3.arff";

string reference_Pic_Names[NBICONS] = { "Accident", "Bomb", "Car", "Casualty", "Electricity", "Fire", "FireBrigade", "Flood", "Gas", "Injury", "Paramedics", "Person", "Police", "RoadBlock"};
string featureName[NBFEATURES] = {"feat1", "feat2", "feat3", "feat4", "feat5", "feat6", "feat7", "feat8", "feat9"};

Mat preprocessing(Mat im);
int computeFeatures(int tab[]);

int main(int argc, char *argv[])
{

    //Open ARFF File
    ofstream file;
    file.open(pathToArff, ios::out | ios::ate );

    if(file) {  // If file exists

        //Initialize ARFF File
        file << "@RELATION imagette" << endl << endl;
        file << "@ATTRIBUTE label {Accident, Bomb, Car, Casualty, Electricity, Fire, FireBrigade, Flood, Gas, Injury, Paramedics, Person, Police, RoadBlock}" << endl;

        for(int i=0; i<NBFEATURES; i++){
            file << "@ATTRIBUTE " << featureName[i] << " numeric" << endl;
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
                    for (int i = 0; i < 2 - pageStringLength; i++) {
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
                                int featureResults[100];
                                int nbResults = computeFeatures(featureResults);

                                // ADD VALUE TO THE ARFF FILE
                                for(int k=0; k<nbResults; k++){
                                    sstm << ", " << featureResults[k];
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

int computeFeatures(int tab[]){
    for(int i=0; i<9; i++){
        tab[i]=i;
    }
    return 9;
}
