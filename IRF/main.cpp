#include "main.h"
#include "omp.h"

#define testBase

int main (void) {
    string inputPath, outputPath;
    int totalNbScripters, totalNbPages, firstScript, firstPage;
    string imgFormat = ".png";

#ifdef testBase
    inputPath = "../donnees/";
    outputPath = "../outputDonnee/";
    firstScript = 1;
    firstPage = 1;
    totalNbScripters = 6;
    totalNbPages = 2;
#endif

#ifndef testBase
    inputPath = "../test/all-scans/w";
    outputPath = "../output/";
    firstScript = 0;
    totalNbScripters = 35;
    totalNbPages = 22;
#endif

    // Get cross reference image ----------------------------
    Mat refCrosses =loadImage("../reference/Cross-small.png");

    // Get icones correspondance images ----------------------------
    int correspondant_Ref_Pic[NBROW];
    string reference_Pic_Names[NBICONREF+1] = { "Accident", "Bomb", "Car", "Casualty", "Electricity", "Fire", "FireBrigade", "Flood", "Gas", "Injury", "Paramedics", "Person", "Police", "RoadBlock", "Car" };
    Mat reference_Pic_RGB[NBICONREF];
    loadReferenceImages(reference_Pic_Names, reference_Pic_RGB, NBICONREF); // Loads reference images from files


    for (int scripterId = firstScript; scripterId < firstScript+totalNbScripters; scripterId++) {
        string scripterString = to_string(scripterId);
        int scripterStringLength = scripterString.length();
        for (int i = 0; i < 3 - scripterStringLength; i++) {
            scripterString = "0" + scripterString;
        }
        string suffixPath = "-scans/" + scripterString;

#pragma omp parallel for
        for (int pageId = firstPage; pageId < firstPage+totalNbPages; pageId++) {
            string pageString = to_string(pageId);

            int pageStringLength = pageString.length();
            for (int i = 0; i < 2 - pageStringLength; i++) {
                pageString = "0" + pageString;
            }

            // Rotate scanned page ----------------------------
            Mat src_rot;
#ifndef testBase
            bool crossesFound = getImageCorrectlyRotated(inputPath + scripterString + suffixPath + pageString + imgFormat, refCrosses, &src_rot);//Get major angle
#else
            string imagePath = inputPath + "s0" + to_string(scripterId) + "_00" + pageString + imgFormat;
            bool crossesFound = getImageCorrectlyRotated(imagePath, refCrosses, &src_rot);//Get major angle
#endif
            if(crossesFound){
                // Get icones correspondance ----------------------------
                Mat src_rot_refPic[7];
                getSmallPicRef(src_rot, src_rot_refPic);
                getCorrespondanceToRefPic(reference_Pic_RGB, src_rot_refPic, correspondant_Ref_Pic);

                //                for (int i = 0; i < NBROW; i++){
                //                    cout << "Picture " << i << " is :" << reference_Pic_Names[correspondant_Ref_Pic[i]] << endl;
                //                }

                // Parse and write imagettes and descriptions ----------------------------
                getAllImagettes(outputPath, src_rot, reference_Pic_Names, correspondant_Ref_Pic, scripterString, pageString, imgFormat);
#ifndef testBase
                for (int i = 0; i < NBROW; i++) {
                    for (int j = 0; j < NBCOLUMN; j++) {
                        writeDescriptionFile(outputPath, reference_Pic_Names[correspondant_Ref_Pic[i]], scripterString, pageString, i, j, MEDIUM_SIZE);
                    }
                }
#endif

                cout << "Page result saved : " << outputPath << scripterString << suffixPath << pageString << imgFormat << " (" << scripterId << "/" << totalNbScripters << ")" << endl;
            }
        }
    }

    waitKey(0);
}

