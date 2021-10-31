// EmagTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


// merge text from first_branch

// merge from second branch

#include <iostream>

//
//#include <algorithm>
//#include <iostream>
//#include <list>
#include <numeric>
//#include <random>
#include <vector>

//header files
#include "../ImageInformation/ImageInformation.h"
#include "../RepeatedObjects/RepeatedObjects.h"

// opencv 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

// boost


using namespace std;
using namespace cv;

int main()
{
    string SrcPath = "U:/DevInsulatorInspection/image/sample/typeC/";
    string Filename = "ce_ins_in05.jpg"; //ins0215.jpg
    string ImageFile = SrcPath + Filename;
    
    Mat ImgOriginal;

    ImgOriginal = imread(ImageFile, 1);

    ImageInformation ImgInfo;
    ImgInfo.setImageInfo(ImgOriginal);
    int ImgHeight = ImgInfo.getImageHeight();
    int ImgWidth = ImgInfo.getImageWidth();

    //std::cout << "Image Info ...." << endl;
    //std::cout << "Image  Width : " << ImgWidth << endl;
    //std::cout << "Image Height : " << ImgHeight << endl;

    //namedWindow("Original", 1);
    //imshow("Original", ImgOriginal);


    RepeatedObjects RepeatedObj;
    RepeatedObj.ImageResize(ImgOriginal);
    //namedWindow("Enhanced", 1);
    //imshow("Enhanced", ImgOriginal);

    Mat ImgGray, ImgSegmented, ImgBinary;

    cvtColor(ImgOriginal, ImgGray, COLOR_BGR2GRAY);
    ImgSegmented = ImgGray;
    try 
    {
        RepeatedObj.ImageSegmentation(ImgSegmented, 25);
    }
    catch (const char* msg)
    {
        cerr << msg << endl;
    }


    // contour detection
    inRange(ImgSegmented, 220, 255, ImgBinary);
    namedWindow("Binary Image", 1);
    imshow("Binary Image", ImgBinary);

    
    
    RepeatedObj.SortedContours(ImgBinary);
    //namedWindow("Sorted Contour", 1);
    //imshow("Sorted Contour", ImgBinary);

    line(ImgOriginal, RepeatedObj.LineX1Y1, RepeatedObj.LineX2Y2, Scalar(255, 0, 0), 2, LINE_4);
    namedWindow("Line", 1);
    imshow("Line", ImgOriginal);


    waitKey(0);
    return 0;
}
