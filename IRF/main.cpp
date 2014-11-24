#include "main.h"


int main (void) {
	string filePath = "../test/all-scans/w";
	string scripterNumber = "000";
	string suffixPath = "-scans/" + scripterNumber;
	string pageNumber = "04";
	string imgFormat = ".png";

	// Rotate scanned page ----------------------------
    Mat src_rot = getImageCorrectlyRotated(filePath + scripterNumber + suffixPath + pageNumber + imgFormat);
    Mat src_rot_refPic[7];
    getSmallPicRef(src_rot, src_rot_refPic);
	
	// Get icones correspondance ----------------------------
    int correspondant_Ref_Pic[NBROW];
    string reference_Pic_Names[NBICONREF] = {"Accident", "Bomb", "Car", "Casualty", "Electricity", "Fire", "FireBrigade", "Flood", "Gas", "Injury", "Paramedics", "Person", "Police", "RoadBlock"};
    Mat reference_Pic_RGB[NBICONREF];
    loadReferenceImages(reference_Pic_Names, reference_Pic_RGB, NBICONREF); // Loads reference images from files
	
    getCorrespondanceToRefPic(reference_Pic_RGB, src_rot_refPic, correspondant_Ref_Pic);
	
    for(int i=0; i<NBROW; i++){
        cout << "Picture " << i << " is :" << reference_Pic_Names[correspondant_Ref_Pic[i]] << endl;
    }
	
	// Parse and write imagettes ----------------------------
    for(int i = 0; i < NBROW; i++) {
		for (int j = 0; j < NBCOLUMN; j++) {
			// TODO: get imagette output
			writeImagetteFile(reference_Pic_Names[correspondant_Ref_Pic[i]], scripterNumber, pageNumber, i, j, imgFormat);
			writeDescriptionFile(reference_Pic_Names[correspondant_Ref_Pic[i]], scripterNumber, pageNumber, i, j, MEDIUM_SIZE);
		}
	}
	
    waitKey(0);
}

