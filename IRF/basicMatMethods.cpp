#include "basicMatMethods.h"

Mat loadImage(string imName){
    // Charge et affiche l'image
    Mat im = imread(imName);
    if(im.data == NULL){
        cerr << "Image not found: "<< imName << endl;
        exit(0);
    }
    return im;
}

Mat getSmallerImage(Mat inputImage, int sizeReduction){
    Size tailleReduite(inputImage.cols/sizeReduction, inputImage.rows/sizeReduction);
    Mat imreduite = Mat(tailleReduite,CV_8UC3);

    resize(inputImage,imreduite,tailleReduite);
    return imreduite;
}

void loadReferenceImages(string inputPaths[], Mat outputTMat[], int nb){
    for(int i=0; i<nb;i++){
        outputTMat[i] = loadImage("../reference/"+inputPaths[i]+"2.png");
    }
}

void splitImage(Mat src, Mat output[], int height){
    for(int i=0; i<7; i++){
        if(i*height < src.rows){
            cv::Rect myROI(0, i*height, src.cols-1, height);
            output[i]=src(myROI);
        }
    }
}
