#include "main.h"
#include "omp.h"


int main (void) {
    string inputPath = "../test/all-scans/w";
    string outputPath = "../output/";
    string imgFormat = ".png";
    int totalNbScripters = 35;
    int totalNbPages = 22;

    // Get cross reference image ----------------------------
    Mat refCrosses =loadImage("../reference/Cross-small.png");

    // Get icones correspondance images ----------------------------
    int correspondant_Ref_Pic[NBROW];
    string reference_Pic_Names[NBICONREF+1] = { "Accident", "Bomb", "Car", "Casualty", "Electricity", "Fire", "FireBrigade", "Flood", "Gas", "Injury", "Paramedics", "Person", "Police", "RoadBlock", "Car" };
    Mat reference_Pic_RGB[NBICONREF];
    loadReferenceImages(reference_Pic_Names, reference_Pic_RGB, NBICONREF); // Loads reference images from files


    for (int scripterId = 0; scripterId < totalNbScripters; scripterId++) {
        string scripterString = to_string(scripterId);
        int scripterStringLength = scripterString.length();
        for (int i = 0; i < 3 - scripterStringLength; i++) {
            scripterString = "0" + scripterString;
        }
        string suffixPath = "-scans/" + scripterString;

        #pragma omp parallel for
        for (int pageId = 0; pageId < totalNbPages; pageId++) {
            string pageString = to_string(pageId);

            int pageStringLength = pageString.length();
            for (int i = 0; i < 2 - pageStringLength; i++) {
                pageString = "0" + pageString;
            }

            // Rotate scanned page ----------------------------
            Mat src_rot;
            bool crossesFound = getImageCorrectlyRotated(inputPath + scripterString + suffixPath + pageString + imgFormat, refCrosses, &src_rot);//Get major angle

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
                for (int i = 0; i < NBROW; i++) {
                    for (int j = 0; j < NBCOLUMN; j++) {
                        writeDescriptionFile(outputPath, reference_Pic_Names[correspondant_Ref_Pic[i]], scripterString, pageString, i, j, MEDIUM_SIZE);
                    }
                }

                cout << "Page result saved : " << outputPath << scripterString << suffixPath << pageString << imgFormat << " (" << scripterId << "/" << totalNbScripters << ")" << endl;
            }
        }
    }

    waitKey(0);
}

