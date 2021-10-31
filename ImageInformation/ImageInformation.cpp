#include "ImageInformation.h"


ImageInformation::ImageInformation()
{

}

void ImageInformation::setImageInfo(const Mat& Image)
{
    mRowHeight = Image.rows;
    mColWidth = Image.cols;
}

int ImageInformation::getImageWidth()
{
    return mColWidth;
}

int ImageInformation::getImageHeight()
{
    return mRowHeight;
}

ImageInformation::~ImageInformation()
{
}