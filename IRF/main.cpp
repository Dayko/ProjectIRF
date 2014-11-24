#include "rotate_picture.h"




int main (void) {

    Mat src_rot = getImageCorrectlyRotated("../test/all-scans/w000-scans/00004.png");
    Mat src_rot_refPic[7];
    getSmallPicRef(src_rot, src_rot_refPic);


    int correspondant_Ref_Pic[NBROW];
    string reference_Pic_Names[NBICONREF] = {"Accident", "Bomb", "Car", "Casualty", "Electricity", "Fire", "Fire brigade", "Flood", "Gas", "Injury", "Paramedics", "Person", "Police", "Road block"};
    Mat reference_Pic_RGB[NBICONREF];
    loadReferenceImages(reference_Pic_Names, reference_Pic_RGB, NBICONREF); // Loads reference images from files

    getCorrespondanceToRefPic(reference_Pic_RGB, src_rot_refPic, correspondant_Ref_Pic);

    for(int i=0; i<NBROW; i++){
        cout << "Picture " << i << " is :" << reference_Pic_Names[correspondant_Ref_Pic[i]] << endl;
    }

    waitKey(0);
}

