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
#include <algorithm>
#include <numeric>

#include <opencv2/opencv.hpp>

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace cv;

const cv::SIFT& IMProc::sifter() {
    static cv::SIFT* sift = new cv::SIFT(0,3,0.04,10,1);
    return *sift;
}

void showContours(Mat const image, vector<vector<Point>> const contours) {
    Mat contoursImage;
    image.copyTo(contoursImage);
    drawContours(contoursImage, contours, -1, Scalar(255), 5);
    MYSHOWSMALL(contoursImage,1);
}

vector<Point> IMProc::findScreenContour(Mat const& image) {
    
    Mat blurredCopy;
    GaussianBlur(image, blurredCopy, Size(5,5), 2);
    
    Mat cannyCopy;
    Canny(blurredCopy, cannyCopy, 60, 200, 3, true);
    
    Mat contourCopy;
    cannyCopy.copyTo(contourCopy);
    vector<vector<Point>> contours;
    findContours(contourCopy, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    //TODO handle case where nothing is found
    
    sort(contours.begin(), contours.end(), [](vector<Point> &left, vector<Point> &right){
        return contourArea(left) > contourArea(right);
    });
    transform(contours.begin(), contours.end(), contours.begin(), [](vector<Point> contour){
        double perimeter = arcLength(contour, true);
        vector<Point> approximatePoints;
        approxPolyDP(contour, approximatePoints, 0.02*perimeter, true);
        return approximatePoints;
    });
    
    vector<Point> screenContour = *find_if(contours.begin(), contours.end(), [](vector<Point> contour){
        return contour.size() == 4;
    });
    
    Point sum = accumulate(screenContour.begin(), screenContour.end(), Point(0,0));
    Point center(sum.x/screenContour.size(), sum.y/screenContour.size());

    vector<Point> orientedScreenContour(4);
    for (auto&& point : screenContour) {
        bool toLeft = point.x < center.x;
        bool above = point.y < center.y;
        if (toLeft && above) {
            orientedScreenContour[0] = point;
        }
        if (!toLeft && above) {
            orientedScreenContour[3] = point;
        }
        if (!toLeft && !above) {
            orientedScreenContour[2] = point;
        }
        if (toLeft && !above) {
            orientedScreenContour[1] = point;
        }
    }
    return orientedScreenContour;
}

Mat IMProc::colorImageToBoard(Mat const& colorBoardImage) {
    Mat greyBoardImage;
    Mat screenImage;
    Mat outputImage;
    Mat contoursImage;
    colorBoardImage.copyTo(contoursImage);
    
    cvtColor(colorBoardImage, greyBoardImage, CV_RGB2GRAY);
    
    vector<Point> screenContour = IMProc::findScreenContour(greyBoardImage);
    //TODO: handle empty screenContour

    const Point2f fromCameraPoints[4] = {screenContour[0], screenContour[1], screenContour[2], screenContour[3]};
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    
    warpPerspective(greyBoardImage, screenImage, getPerspectiveTransform(fromCameraPoints, toPoints), Size(800,800));
    
    
    const int leftEdge = 100;
    const int bottomEdge = 670;
    const int topEdge = 220;
    const int rightEdge = 700;
    const Point2f fromScreenPoints[4] = {
        {leftEdge,topEdge},
        {leftEdge,bottomEdge},
        {rightEdge,bottomEdge},
        {rightEdge,topEdge}
    };
    
    warpPerspective(screenImage, outputImage, getPerspectiveTransform(fromScreenPoints, toPoints), Size(800,800));
    
    return outputImage;
}


int IMProc::tileValue(Mat tileImage, const vector<TileInfo>& canonicalTiles) {
    BFMatcher matcher;
    
    vector<KeyPoint> tileKeypoints;
    Mat tileDescriptors;
    
    IMProc::sifter().detect(tileImage, tileKeypoints);
    IMProc::sifter().compute(tileImage, tileKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        //Probably blank
        return 0;
    }
    
    vector<float> distances;
    float min = INFINITY;
    const TileInfo *bestMatch = &canonicalTiles[0];
    
    for (auto&& canonicalTile : canonicalTiles) {
        vector<DMatch> matches;
        matcher.match(canonicalTile.descriptors, tileDescriptors, matches);
        //matcher.knnMatch(canonicalTile.descriptors, tileDescriptors, matches, 2);
        //TODO: multiple matches to the same index invalid
        
        //Ratio test
        
//        vector<DMatch> good_matches;
//        vector<DMatch> bad_matches;
//        for (int i = 0; i < matches.size(); ++i) {
//            const float ratio = 0.8;
//            if (matches[i].size() > 1) {
//                if (matches[i][0].distance < ratio * matches[i][1].distance) {
//                    good_matches.push_back(matches[i][0]);
//                } else {
//                    bad_matches.push_back(matches[i][0]);
//                }
//            } else {
//            }
//        }
        
        
        float averageDistance = accumulate(matches.begin(), matches.end(), float(0), [](float sum, DMatch d) {
            return sum + d.distance;
        })/float(matches.size());
        
        if (averageDistance < min) {
            min = averageDistance;
            bestMatch = &canonicalTile;
        }
        
        
        Mat matchDrawing;
        drawMatches(canonicalTile.image, canonicalTile.keypoints, tileImage, tileKeypoints, matches, matchDrawing);
        MYSHOW(matchDrawing);
        
//        Mat goodMatchDrawing;
//        drawMatches(canonicalTile.image, canonicalTile.keypoints, tileImage, tileKeypoints, good_matches, goodMatchDrawing);
//        MYSHOW(goodMatchDrawing);
//        
//        Mat badMatchDrawing;
//        drawMatches(canonicalTile.image, canonicalTile.keypoints, tileImage, tileKeypoints, bad_matches, badMatchDrawing);
//        MYSHOW(badMatchDrawing);
    }
    
    return bestMatch->value;
}

array<array<Mat, 4>, 4> IMProc::tileImages(Mat boardImage) {
    array<array<Mat, 4>, 4> result;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            result[j][i] = boardImage(Rect(200*i, 200*j, 200, 200));
        }
    }
    return result;
}

array<array<unsigned int, 4>, 4> IMProc::boardState(Mat boardImage, const vector<TileInfo>& canonicalTiles) {
    array<array<Mat, 4>, 4> tiles = tileImages(boardImage);
    array<array<unsigned int, 4>, 4> board;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            board[j][i] = IMProc::tileValue(tiles[j][i], canonicalTiles);
        }
    }
    return board;
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
    
    results.push_back(TileInfo(image1, 1));
    results.push_back(TileInfo(image2, 2));
    
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
    
    results.pop_back(); //Remove the empty spot wher 6144 will eventually go
    
    return results;
}