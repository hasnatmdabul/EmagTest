#pragma once


// opencv 
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

// boost


using namespace std;
using namespace cv;


class ImageInformation
{

private:
    int mRowHeight = 0, mColWidth = 0;

public:

    ImageInformation();
    void setImageInfo(const Mat& Image);
    int getImageWidth();
    int getImageHeight();
    ~ImageInformation();
};

