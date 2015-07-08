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

#include <boost/mpl/transform.hpp>
#include <opencv2/opencv.hpp>

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace cv;
using namespace boost;

MatchResult::MatchResult(TileInfo matchedTile, cv::Mat matchDrawing, vector<DMatch> matches, float averageDistance, float matchingKeypointsFraction) : tile(matchedTile), drawing(matchDrawing), matches(matches), averageDistance(averageDistance), matchingKeypointFraction(matchingKeypointsFraction){
}

const map<int, TileInfo>* loadCanonicalTiles() {
    Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Tiles.png", 0);
    Mat t;
    
    Mat image12 = imread("/Users/drewgross/Projects/ThreesAI/SampleData/12.png", 0);
    Mat t2;
    
    map<int, TileInfo>* results = new map<int, TileInfo>();
    
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
    
    results->emplace(piecewise_construct, forward_as_tuple(1), forward_as_tuple(image1, 1));
    results->emplace(piecewise_construct, forward_as_tuple(2), forward_as_tuple(image2, 2));
    
    const int L = 80;
    const int R = 560;
    const int T = 310;
    const int B = 800;
    
    const Point2f fromPoints[4] = {{L,T},{L,B},{R,B},{R,T}};
    const Point2f toPoints[4] = {{0,0},{0,600},{800,600},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    
    warpPerspective(image, t, transform, Size(800,600));
    
    
    const std::array<int, 13> indexToTile = {1,2,3,6,12,24,48,96,192,384,768,1536,3072};
    
    for (unsigned char i = 0; i < 3; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Mat tile;
            t(Rect(200*j, 200*i, 200, 200)).copyTo(tile);
            results->emplace(piecewise_construct, forward_as_tuple(indexToTile[results->size()]), forward_as_tuple(tile, indexToTile[results->size()]));
        }
    }
    
    results->erase(6144); //Remove the empty spot where 6144 will eventually go
    
    return results;
}

const cv::SIFT& IMProc::sifter() {
    static cv::SIFT* sift = new cv::SIFT(
      Paramater::siftFeatureCount,
      Paramater::siftOctaveLayers,
      Paramater::siftContrastThreshold,
      Paramater::siftEdgeThreshold,
      Paramater::siftGaussianSigma);
    return *sift;
}

const map<int, TileInfo>& IMProc::canonicalTiles() {
    static const map<int, TileInfo>* tiles = loadCanonicalTiles();
    return *tiles;
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
    Canny(blurredCopy, cannyCopy, Paramater::cannyRejectThreshold, Paramater::cannyAcceptThreshold, Paramater::cannyApertureSize, Paramater::cannyUseL2);
    
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
    
    cvtColor(colorBoardImage, greyBoardImage, CV_RGB2GRAY);
    
    vector<Point> screenContour = IMProc::findScreenContour(greyBoardImage);
    //TODO: handle empty screenContour

    //showContours(colorBoardImage, {screenContour});
    
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

float matchNonMatchRatio(vector<KeyPoint> const& queryKeypoints, vector<KeyPoint> const& trainKeypoints, vector<DMatch> const& matches) {
    return float(matches.size())/(queryKeypoints.size()+trainKeypoints.size());
}

MatchResult::MatchResult(TileInfo candidate, Mat image) : tile(candidate) {
    BFMatcher matcher(IMProc::Paramater::tileMatcherNormType, IMProc::Paramater::tileMatcherCrossCheck);
    vector<KeyPoint> tileKeypoints;
    Mat tileDescriptors;
    
    IMProc::sifter().detect(image, tileKeypoints);
    IMProc::sifter().compute(image, tileKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        this->averageDistance = INFINITY;
        this->matches = {};
        
        Mat mdrawing;
        drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, this->matches, mdrawing);
        this->drawing = mdrawing;
        return;
    }
    
    vector<vector<DMatch>> knnMatches;
    matcher.knnMatch(candidate.descriptors, tileDescriptors, knnMatches, 2);

    //Ratio test
    vector<DMatch> ratioPassingMatches;
    for (int i = 0; i < knnMatches.size(); ++i) {
        if (knnMatches[i].size() > 1) {
            if (knnMatches[i][0].distance < IMProc::Paramater::tileMatchRatioTestRatio * knnMatches[i][1].distance) {
                ratioPassingMatches.push_back(knnMatches[i][0]);
            }
        } else if (knnMatches[i].size() == 1) {
            ratioPassingMatches.push_back(knnMatches[i][0]);
        }
    }
    
    //Remove matches the have the same query or train index
    vector<DMatch> noDupeMatches;
    for (auto&& queryMatch : ratioPassingMatches) {
        bool passes = true;
        for (auto&& otherMatch : ratioPassingMatches) {
            if (&queryMatch != &otherMatch) {
                if (queryMatch.queryIdx == otherMatch.queryIdx || queryMatch.trainIdx == otherMatch.trainIdx) {
                    passes = false;
                }
            }
        }
        if (passes) {
            noDupeMatches.push_back(queryMatch);
        }
    }
    
    this->matches = noDupeMatches;
    if (noDupeMatches.size() == 0) {
        this->averageDistance = INFINITY;
    } else {
        this->averageDistance = accumulate(this->matches.begin(), this->matches.end(), float(0), [](float sum, DMatch d) {
            return sum + d.distance;
        })/this->matches.size();
    }
    this->matchingKeypointFraction = matchNonMatchRatio(candidate.keypoints, tileKeypoints, noDupeMatches);
    
    Mat mdrawing;
    drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, this->matches, mdrawing);
    this->drawing = mdrawing;
}

MatchResult IMProc::tileValue(const Mat& tileImage, const map<int, TileInfo>& canonicalTiles) {
    Mat tileDescriptors;
    vector<KeyPoint> tileKeypoints;
    
    IMProc::sifter().detect(tileImage, tileKeypoints);
    IMProc::sifter().compute(tileImage, tileKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        //Probably either a zero or a 1, guess based on image variance
        Scalar mean;
        Scalar stdDev;
        meanStdDev(tileImage, mean, stdDev);;
        if (stdDev[0] < Paramater::zeroOrOneStdDevThreshold) {
            return MatchResult(TileInfo(Mat(), 0), tileImage, {}, INFINITY, INFINITY);
        } else {
            return MatchResult(TileInfo(Mat(), 1), tileImage, {}, INFINITY, INFINITY);
        }
    }
    
    vector<MatchResult> matchResults;
    for (auto&& t : canonicalTiles) {
        matchResults.emplace_back(t.second, tileImage);
    }
    
    sort(matchResults.begin(), matchResults.end(), [](MatchResult l, MatchResult r){
        return l.averageDistance < r.averageDistance;
    });
    
    vector<MatchResult> goodMatchResults;
    for (auto&& m : matchResults) {
        if (m.matchingKeypointFraction > 0.055) {
            goodMatchResults.push_back(m);
        }
    }
    
    if (goodMatchResults.empty()) {
        return matchResults[0];
    }
    
    float lowestAverageDistance = goodMatchResults[0].averageDistance;
    auto potentailMatchEnd = goodMatchResults.begin();
    while (potentailMatchEnd != goodMatchResults.end() && potentailMatchEnd->averageDistance < lowestAverageDistance*Paramater::goodEnoughAverageMultiplier) {
        potentailMatchEnd++;
    }
    
    sort(goodMatchResults.begin(), potentailMatchEnd, [](MatchResult l, MatchResult r){
        return l.matchingKeypointFraction > r.matchingKeypointFraction;
    });
    
    return goodMatchResults[0];
}

array<Mat, 16> IMProc::tileImages(Mat boardImage) {
    array<Mat, 16> result;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            result[i+j*4] = boardImage(Rect(200*i, 200*j, 200, 200));
        }
    }
    return result;
}

array<unsigned int, 16> IMProc::boardState(Mat boardImage, const map<int, TileInfo>& canonicalTiles) {
    array<unsigned int, 16> result;
    array<Mat, 16> images = tileImages(boardImage);
    transform(images.begin(), images.end(), result.begin(), [&canonicalTiles](Mat image){
        return tileValue(image, canonicalTiles).tile.value;
    });
    return result;
}
