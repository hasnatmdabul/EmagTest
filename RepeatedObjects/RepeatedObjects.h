#pragma once

#include <iostream>
#include <vector>
#include <numeric>

#include "../ImageInformation/ImageInformation.h"


class RepeatedObjects : public ImageInformation
{

private:
    size_t ResizeFactor = 0.0;

    int mMinConSize = 75;
    int mMaxConSize = 150;
    int mMinDistVote = 12;      // minimum distance between ?
    int mMaxWidthHeight = 80;   // Max height or width
    int mMinWidthHeight = 30;   // Min Width or height
    int mMaxGapBetPins = 80;     // maximum gap between insulator pins , e.g for voting

    vector<vector<Point> > contours = {};
    vector<Vec4i> hierarchy = {};


    vector<double> contourCenterX;
    vector<double> contourCenterY;

    //PCA
    vector<vector<double>> DistanceAll;     //distance of all points from each point
    vector<double> DistancePP;               //Distance point to point
    //RANSAC
    vector<vector<int>> VoteAll;
    vector<int> voteTemp;
    // Object Location
    double LineAX=0, LineAY=0, LineBX=0, LineBY = 0;
    size_t length;


public:
    RepeatedObjects();

    // Line 
    Point2d LineX1Y1;
    Point2d LineX2Y2;


    void ImageResize(Mat& Image);
    void ImageSegmentation(Mat& GrayImage, int HistoBinDiff);
    void FilterByConSize(std::vector<vector<Point> >& contours, int min, int max);
    void FilterByConHW(std::vector<vector<Point> >& contours);
    void FilterByDistantCon(std::vector<vector<Point> >& contours);
    void RANSAC(std::vector<vector<Point> >& contours);
    void ObjLocation(std::vector<vector<Point> >& contours);
    void DeleteNoteOffDist(vector<vector<double> >& eventStore, double x);
    double pDistance(double x, double y, double x1, double y1, double x2, double y2);
    void SortedContours(Mat& ContourBinaryImage);

    ~RepeatedObjects();
};


// TO Do: make template fpr point and double
//void DeleteNoteOff(std::vector<vector<Point> >& eventStore)
//{
//    eventStore.erase(std::remove_if(eventStore.begin(), eventStore.end(),
//        [](const std::vector<Point>& v) {return v.size() < 110 || v.size() > 150; }),
//        eventStore.end());
//}