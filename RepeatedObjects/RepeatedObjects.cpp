#include "RepeatedObjects.h"


RepeatedObjects::RepeatedObjects()
{

}


void RepeatedObjects::ImageResize(Mat& Image)
{
    //double ScaleFrameSize = 1.0; // ResizeFactor between 0 and 1
    if ((Image.rows > 1000 && Image.rows < 2000) || (Image.cols > 1000 && Image.cols < 2000))
    {
        //ScaleFrameSize = 0.5;
        Image.rows = Image.rows / 2;
        Image.cols = Image.cols / 2;
    }
    else if (Image.rows > 2000 || Image.cols > 2000)
    {
        //ScaleFrameSize = 0.3;
        Image.rows = Image.rows / 3;
        Image.cols = Image.cols / 3;
    }
    //int Cols = int(Image.cols * ScaleFrameSize);
    //int Rows = int(Image.rows * ScaleFrameSize);

    resize(Image, Image, Size(Image.cols, Image.rows), 0, 0, INTER_LINEAR);
}


void RepeatedObjects::ImageSegmentation(Mat& GrayImage, int HistoBinDiff)
{
    if (HistoBinDiff < 1 || HistoBinDiff > 255)
    {
        throw " Number between 1 to 255 !";
    }

    // HistoBinDiff group diff must be even number and more then 4
    int  halfValue = 0;
    int AdjustCurrentPixel = 0;
    int CurrentPixelValue = 0;

    int rows = GrayImage.rows;
    int cols = GrayImage.cols;

    for (int i = 2; i < rows - 2; i++)
    {
        for (int j = 2; j < cols - 2; j++)
        {

            if (GrayImage.at<uchar>(i, j) < 51 || GrayImage.at<uchar>(i, j) > (255 - HistoBinDiff))  // continue for darest or brightest pixel
            {
                continue;
            }

            //halfValue = round(HistoBinDiff / 2);
            halfValue = HistoBinDiff / 2;
            AdjustCurrentPixel = GrayImage.at<uchar>(i, j) % HistoBinDiff;
            CurrentPixelValue = GrayImage.at<uchar>(i, j);

            if (AdjustCurrentPixel >= halfValue) //  && CurrentPixelValue < 250
            {
                GrayImage.at<uchar>(i, j) = CurrentPixelValue + (HistoBinDiff - AdjustCurrentPixel);
            }
            else if (AdjustCurrentPixel < halfValue) // && CurrentPixelValue > 10
            {
                GrayImage.at<uchar>(i, j) = CurrentPixelValue - AdjustCurrentPixel;
            }

        }

    }

}


void RepeatedObjects::FilterByConSize(std::vector<vector<Point> >& contours, int min, int max)
{
    // remove contours by size (number of pixels)
    contours.erase
    (
        std::remove_if(contours.begin(), contours.end(),
        [min, max](const std::vector<Point>& v) // lambda (& also possible in min and max)
        {
            return v.size() < min || v.size() > max; 
        }),
        contours.end());
}


void RepeatedObjects::FilterByConHW(std::vector<vector<Point> >& contours)
{
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    int centerX = 0;
    int centerY = 0;
    Point2f contourCircleCenterXY;
    Scalar color;
    RNG rng(12345); // contour
    //Mat SelectedContours = Mat::zeros(ContourBinaryImage.size(), CV_8UC3);
    Mat SelectedContours = Mat::zeros( getImageHeight(), getImageWidth(), CV_8UC3); // get image height and width from ImageInformation class

    for (size_t i = 0; i < contours.size(); i++)
    {
        color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
        approxPolyDP(contours[i], contours_poly[i], 3, true);
        boundRect[i] = boundingRect(contours_poly[i]);
        drawContours(SelectedContours, contours_poly, (int)i, color);

        centerX = boundRect[i].tl().x + (boundRect[i].width / 2);
        centerY = boundRect[i].tl().y + (boundRect[i].height / 2);

        if (boundRect[i].width > mMaxWidthHeight || boundRect[i].height > mMaxWidthHeight || boundRect[i].height < mMinWidthHeight || boundRect[i].height < mMinWidthHeight)
        {
            contours.erase(contours.begin() + i);
            i = i - 1;
            continue;
        }

        // Circles
        contourCenterX.push_back(centerX);
        contourCenterY.push_back(centerY);
        contourCircleCenterXY = Point2f(centerX, centerY);
        int radius = 10;
        //circle(SelectedContours, CenterXY, radius, color);
    }

}


void RepeatedObjects::FilterByDistantCon(std::vector<vector<Point> >& contours)
{
    // ToDo  : declared double, class member and function
    vector<vector<double>> OldDistanceAll;     //distance of all points from each point
    vector<double>OldDistancePP;               //Distance point to point

    for (size_t i = 0; i < contourCenterX.size(); i++)
    {
        for (size_t j = 0; j < contourCenterX.size(); j++)
        {

            double testX2 = contourCenterX.at(j);
            double testX1 = contourCenterX.at(i);
            double testY2 = contourCenterY.at(j);
            double testY1 = contourCenterY.at(i);
            double distance = sqrt(pow((testX2 - testX1), 2) + pow((testY2 - testY1), 2));

            //double distance = sqrt(pow((AllCenterX.at(i) - AllCenterX.at(j)), 2) + pow((AllCenterY.at(i) - AllCenterY.at(j)), 2));
            OldDistancePP.push_back(distance);
        }

        //sorting DistancePP index
        OldDistanceAll.push_back(OldDistancePP);
        OldDistancePP = {};
    }

    // PCA and RANSAC
    vector<double> SortedIdxDistancePP(OldDistanceAll.size());
    iota(SortedIdxDistancePP.begin(), SortedIdxDistancePP.end(), 0);
    //vector<double> DistancePP;
    size_t lengthA = contourCenterX.size();
    for (size_t i = 0; i < lengthA - 1; i++)
    {
        OldDistancePP = OldDistanceAll.at(i);
        // low to high distance sort
        stable_sort(SortedIdxDistancePP.begin(), SortedIdxDistancePP.end(), [&OldDistancePP](size_t i1, size_t i2) {return OldDistancePP[i1] < OldDistancePP[i2]; });

        // ToDo : reduce distant points or objects which does not have close neighbour
        int gapBetCons = OldDistanceAll.at(i).at(SortedIdxDistancePP.at(1));
        int testLast = OldDistanceAll.at(i).at(SortedIdxDistancePP.at(OldDistanceAll.size() - 1));

        // remove lonely! points/objects
        if (gapBetCons > mMaxGapBetPins) // assumed distance between insulator pin to neighour pin
        {
            // get rid of any object is far from others, at least 40 pixel
            OldDistanceAll.at(i) = { -1 };
            DeleteNoteOffDist(OldDistanceAll, SortedIdxDistancePP.at(1));    // PrincipleComponentAnalysis -  distance of contours
            contourCenterX.erase(contourCenterX.begin() + i);
            contourCenterY.erase(contourCenterY.begin() + i);
            lengthA = contourCenterX.size();
            i--;

        }

        //cout << DistanceAll.at(i).at(SortedIdxDistancePP.at(1)) << endl;
        OldDistancePP = {};
    }

    DistanceAll = OldDistanceAll;     //distance of all points from each point
    DistancePP = OldDistancePP;               //Distance point to point
}


void RepeatedObjects::RANSAC(std::vector<vector<Point> >& contours)
{

    int voteCount = 0;
    //double tempDistance;
    for (size_t i = 0; i < DistanceAll.size(); i++)
    {
        for (size_t j = 0; j < DistanceAll.size(); j++)
        {
            for (size_t k = 0; k < DistanceAll.size(); k++)
            {
                //x, y is your target point and x1, y1 to x2, y2 is your line segment. : pDistance(x, y, x1, y1, x2, y2)
                double test = pDistance(contourCenterX.at(k), contourCenterY.at(k), contourCenterX.at(j), contourCenterY.at(j), contourCenterX.at(i), contourCenterY.at(i));
                if (test < mMinDistVote)  // perpendicular distance of a point from a line, minimum 20 px
                {
                    voteCount += 1;
                }
            }
            voteTemp.push_back(voteCount);
            voteCount = 0;
        }

        VoteAll.push_back(voteTemp);
        voteTemp = {};
    }

}


void RepeatedObjects::ObjLocation(std::vector<vector<Point> >& contours)
{
    vector<size_t> SortedIdxVoteCurrent(DistanceAll.size());
    vector<int> VoteCurrent;
    iota(SortedIdxVoteCurrent.begin(), SortedIdxVoteCurrent.end(), 0);

    vector<size_t> SortedIdxVoteNext(DistanceAll.size());
    vector<int> VoteNext;
    iota(SortedIdxVoteNext.begin(), SortedIdxVoteNext.end(), 0);

    int currentMax = 0;
    length = contourCenterX.size();
    for (size_t i = 0; i < length - 1; i++)
    {
        VoteCurrent = VoteAll.at(i);
        stable_sort(SortedIdxVoteCurrent.begin(), SortedIdxVoteCurrent.end(), [&VoteCurrent](size_t i1, size_t i2) {return VoteCurrent[i1] < VoteCurrent[i2]; });

        VoteNext = VoteAll.at(i + 1);
        stable_sort(SortedIdxVoteNext.begin(), SortedIdxVoteNext.end(), [&VoteNext](size_t i1, size_t i2) {return VoteNext[i1] < VoteNext[i2]; });

        int testCurrent = VoteAll.at(i).at(SortedIdxVoteCurrent.at(length - 1));
        int testNext = VoteAll.at(i + 1).at(SortedIdxVoteNext.at(length - 1));

        if (testCurrent > testNext && testCurrent > currentMax)
        {
            LineAX = contourCenterX.at(i);
            LineAY = contourCenterY.at(i);
            LineBX = contourCenterX.at(SortedIdxVoteCurrent.at(length - 1));
            LineBY = contourCenterY.at(SortedIdxVoteCurrent.at(length - 1));
            currentMax = testCurrent;
        }
        else if (testNext >= testCurrent && testNext > currentMax)
        {
            LineAX = contourCenterX.at(i);
            LineAY = contourCenterY.at(i);
            LineBX = contourCenterX.at(SortedIdxVoteNext.at(length - 1));
            LineBY = contourCenterY.at(SortedIdxVoteNext.at(length - 1));
            testNext = testCurrent;

        }
    }

}



// TO Do: make template fpr point and double
//void DeleteNoteOff(std::vector<vector<Point> >& eventStore)
//{
//    eventStore.erase(std::remove_if(eventStore.begin(), eventStore.end(),
//        [](const std::vector<Point>& v) {return v.size() < 110 || v.size() > 150; }),
//        eventStore.end());
//}


void RepeatedObjects::DeleteNoteOffDist(vector<vector<double> >& eventStore, double x)
{
    eventStore.erase(std::remove_if(eventStore.begin(), eventStore.end(),
        [](const std::vector<double>& v) {return v[0] < 0; }),              // lambda
        eventStore.end());
}


double RepeatedObjects::pDistance(double x, double y, double x1, double y1, double x2, double y2) {

    double A = x - x1;
    double B = y - y1;
    double C = x2 - x1;
    double D = y2 - y1;

    double dot = A * C + B * D;
    double len_sq = C * C + D * D;
    double param = -1;
    if (len_sq != 0) //in case of 0 length line
        param = dot / len_sq;

    double xx, yy;

    if (param < 0) {
        xx = x1;
        yy = y1;
    }
    else if (param > 1) {
        xx = x2;
        yy = y2;
    }
    else {
        xx = x1 + param * C;
        yy = y1 + param * D;
    }

    double dx = x - xx;
    double dy = y - yy;
    return sqrt(dx * dx + dy * dy);
}


void RepeatedObjects::SortedContours(Mat& ContourBinaryImage)
{


    // find contours
    findContours(ContourBinaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    // get rid of anything < 10
    FilterByConSize(contours, 75, 150);    // PrincipleComponentAnalysis -  size of contours
    // filter objects by MinMax Height and Width
    FilterByConHW(contours);
    // remove distant contours/objects
    FilterByDistantCon(contours);
    //
    RANSAC(contours);
    // Find best location
    ObjLocation(contours);

    // if no contour found
    //size_t Length = AllCenterX.size();
    //if (Length == 0)
    //{
    //    return GrayImage; //if no contour/insulator found then return
    //}
    // PCA(Principal component analysis) Distance
    // Point to point distance , d = root((x1-x2)^2 + (y1-y2)^2)
    // RANSAC -- Distance point to line
    // testing required. 
    //TO Do : testing the process
// draw the best line
// get length from vector , fix it !

    // at least three/3 insulator should be visible in the image
    if (length > 3)
    {
        //for (size_t i = 0; i < length; i++)
        //{
        //    //x, y is your target point and x1, y1 to x2, y2 is your line segment. : pDistance(x, y, x1, y1, x2, y2)
        //    size_t test = pDistance(contourCenterX.at(i), contourCenterY.at(i), LineAX, LineAY, LineBX, LineBY);
        //    cout << test << endl;
        //    if (test < mMinDistVote)
        //    {
        //        circle(ContourBinaryImage, Point2f(contourCenterX.at(i), contourCenterY.at(i)), 10, Scalar(255, 0, 0));
        //    }
        //}
        //line(ContourBinaryImage, Point2f(LineAX, LineAY), Point2f(LineBX, LineBY), Scalar(255, 0, 0), 2, LINE_4);

        LineX1Y1 = Point2d(LineAX, LineAY);
        LineX2Y2 = Point2d(LineBX, LineBY);
    }



    //namedWindow("Binary Image1", 1);
    //imshow("Binary Image1", SelectedContours);
    //ContourBinaryImage = SelectedContours;
}


RepeatedObjects::~RepeatedObjects() {}
