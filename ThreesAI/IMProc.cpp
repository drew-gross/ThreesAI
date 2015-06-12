//
//  IMProc.cpp
//  ThreesAI
//
//  Created by Drew Gross on 6/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "IMProc.h"

#include <vector>
#include <array>

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
    //TODO: handle empty screenContour
    
    const Point2f fromCameraPoints[4] = {screenContour[0], screenContour[1], screenContour[2], screenContour[3]};
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    
    warpPerspective(greyBoardImage, screenImage, getPerspectiveTransform(fromCameraPoints, toPoints), Size(800,800));
    
    const Point2f fromScreenPoints[4] = {{100,210},{100,670},{700,670},{700,210}};
    
    warpPerspective(screenImage, outputImage, getPerspectiveTransform(fromScreenPoints, toPoints), Size(800,800));
    
    return outputImage;
}

const vector<TileInfo> IMProc::loadCanonicalTiles() {
    Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Tiles.png", 0);
    Mat t;
    
    Mat image12 = imread("/Users/drewgross/Projects/ThreesAI/SampleData/12.png", 0);
    Mat t2;
    
    vector<TileInfo> results;
    
    const int L12 = 80;
    const int R12 = 200;
    const int T12 = 310;
    const int B12 = 630;
    
    const Point2f fromPoints12[4] = {{L12,T12},{L12,B12},{R12,B12},{R12,T12}};
    const Point2f toPoints12[4] = {{0,0},{0,400},{200,400},{200,0}};
    warpPerspective(image12, t2, getPerspectiveTransform(fromPoints12, toPoints12), Size(200,400));
    
    Mat image1;
    t2(Rect(0,200,200,200)).copyTo(image1);
    
    Mat image2;
    t2(Rect(0,0,200,200)).copyTo(image2);
    
    results.push_back(TileInfo(image2, 2));
    results.push_back(TileInfo(image1, 1));
    
    const int L = 80;
    const int R = 560;
    const int T = 310;
    const int B = 800;
    
    const Point2f fromPoints[4] = {{L,T},{L,B},{R,B},{R,T}};
    const Point2f toPoints[4] = {{0,0},{0,600},{800,600},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    
    warpPerspective(image, t, transform, Size(800,600));
    
    
    const array<int, 13> indexToTile = {1,2,3,6,12,24,48,96,192,384,768,1536,3072};
    
    for (unsigned char i = 0; i < 3; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Mat tile;
            t(Rect(200*j, 200*i, 200, 200)).copyTo(tile);
            results.push_back(TileInfo(tile, indexToTile[results.size()]));
        }
    }
    
    return results;
}