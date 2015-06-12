//
//  IMProc.cpp
//  ThreesAI
//
//  Created by Drew Gross on 6/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "IMProc.h"

#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Point> IMProc::findScreenContour(Mat image) {
    Mat copy;
    Mat copy2;
    Canny(image, copy, 30, 200);
    copy.copyTo(copy2);
    vector<vector<Point>> contours;
    findContours(copy2, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    
    sort(contours.begin(), contours.end(), [](vector<Point> &left, vector<Point> &right){
        return contourArea(left) > contourArea(right);
    });
    
    vector<Point> screenContour;
    for (auto&& contour : contours) {
        double perimeter = arcLength(contour, true);
        vector<Point> approximatePoints;
        approxPolyDP(contour, approximatePoints, 0.02*perimeter, true);
        
        if (approximatePoints.size() == 4) {
            screenContour = approximatePoints;
            break;
        }
    }
    
    vector<vector<Point>> cs = {screenContour};
    drawContours(image, cs, -1, Scalar(255));
    return screenContour;
}


Mat IMProc::colorImageToBoard(Mat colorBoardImage) {
    Mat greyBoardImage;
    Mat screenImage;
    Mat outputImage;
    
    cvtColor(colorBoardImage, greyBoardImage, CV_RGB2GRAY);
    greyBoardImage = imread("/Users/drewgross/Projects/ThreesAI/SampleData/CameraSample1.png", 0);
    
    vector<Point> screenContour = IMProc::findScreenContour(greyBoardImage);
    
    const Point2f fromCameraPoints[4] = {screenContour[0], screenContour[1], screenContour[2], screenContour[3]};
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    
    warpPerspective(greyBoardImage, screenImage, getPerspectiveTransform(fromCameraPoints, toPoints), Size(800,800));
    
    const Point2f fromScreenPoints[4] = {{100,210},{100,670},{700,670},{700,210}};
    
    warpPerspective(screenImage, outputImage, getPerspectiveTransform(fromScreenPoints, toPoints), Size(800,800));
    
    return outputImage;
}