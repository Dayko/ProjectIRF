#include "preprocessing.h"
#include "normalization.h"


Mat preProcess(Mat inputImage)
{
    Mat im_out=inputImage;

    //Opening to reduce possible noise (erosion+dilation)
    int morph_size = 1;
    Mat element = getStructuringElement(0, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
    morphologyEx(im_out, im_out, 1, element);

    //Closing to strenghten the lines again and to fill unwanted gaps
    morphologyEx(im_out, im_out, 2, element);

    //Median filter
    for (int i = 1; i < 3; i = i + 2)
    {
        medianBlur(im_out, im_out, i);
    }

    Mat im_bounding = BoundingRatio(im_out);

    return im_bounding;
}
