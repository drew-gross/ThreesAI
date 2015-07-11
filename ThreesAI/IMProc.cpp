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
#include <numeric>

#include <opencv2/opencv.hpp>

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace cv;

const Point2f IMProc::getPoint(const string& window) {
    Point2f p;
    setMouseCallback(window, [](int event, int x, int y, int flags, void* userdata){
        Point2f *p = (Point2f*)userdata;
        p->x = x;
        p->y = y;
    }, &p);
    waitKey();
    return p;
}

const std::array<Point2f, 4> IMProc::getQuadrilateral(Mat m) {
    imshow("get rect", m);
    return std::array<cv::Point2f, 4>{{getPoint("get rect"),getPoint("get rect"),getPoint("get rect"),getPoint("get rect")}};
}

const map<int, TileInfo>* loadCanonicalTiles() {
    Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Tiles.png", 0);
    Mat boardImage;
    
    Mat image12 = imread("/Users/drewgross/Projects/ThreesAI/SampleData/12.png", 0);
    Mat board12;
    
    map<int, TileInfo>* results = new map<int, TileInfo>();
    
    const int L12 = 80;
    const int R12 = 200;
    const int T12 = 310;
    const int B12 = 630;
    
    const Point2f fromPoints12[4] = {{L12,T12},{L12,B12},{R12,B12},{R12,T12}};
    const Point2f toPoints12[4] = {{0,0},{0,400},{200,400},{200,0}};
    warpPerspective(image12, board12, getPerspectiveTransform(fromPoints12, toPoints12), Size(200,400));
    
    results->emplace(piecewise_construct, forward_as_tuple(1), forward_as_tuple(IMProc::tileFromIntersection(board12, 0, 200), 1, IMProc::canonicalSifter()));
    results->emplace(piecewise_construct, forward_as_tuple(2), forward_as_tuple(IMProc::tileFromIntersection(board12, 0, 0), 2, IMProc::canonicalSifter()));
    
    const int L = 80;
    const int R = 560;
    const int T = 310;
    const int B = 800;
    
    const Point2f fromPoints[4] = {{L,T},{L,B},{R,B},{R,T}};
    const Point2f toPoints[4] = {{0,0},{0,600},{800,600},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    
    warpPerspective(image, boardImage, transform, Size(800,600));
    
    
    const std::array<int, 14> indexToTile = {1,2,3,6,12,24,48,96,192,384,768,1536,3072,6144};
    
    for (unsigned char i = 0; i < 3; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            int value = indexToTile[results->size()];
            //dirty hack to get an empty tile into the map
            //TODO: fix this.
            if (value == 6144) {
                value = 0;
            }
            
            results->emplace(piecewise_construct, forward_as_tuple(value), forward_as_tuple(IMProc::tileFromIntersection(boardImage, 200*j, 200*i), value, IMProc::canonicalSifter()));
        }
    }
    
    return results;
}

const Mat IMProc::tileFromIntersection(Mat image, int x, int y) {
    return image(Rect(x + Paramater::ignoredEdgePadding,
                      y + Paramater::ignoredEdgePadding,
                      200 - Paramater::ignoredEdgePadding*2,
                      200 - Paramater::ignoredEdgePadding*2));
}

const cv::SIFT& IMProc::canonicalSifter() {
    static cv::SIFT* sift = new cv::SIFT(
        Paramater::canonicalFeatureCount,
        Paramater::canonicalOctaveLayers,
        Paramater::canonicalContrastThreshold,
        Paramater::canonicalEdgeThreshold,
        Paramater::canonicalGaussianSigma);
    return *sift;
}

const cv::SIFT& IMProc::imageSifter() {
    static cv::SIFT* sift = new cv::SIFT(
        Paramater::imageFeatureCount,
        Paramater::imageOctaveLayers,
        Paramater::imageContrastThreshold,
        Paramater::imageEdgeThreshold,
        Paramater::imageGaussianSigma);
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

MatchResult::MatchResult(TileInfo candidate, Mat image, bool calculate) : tile(candidate), knnDrawing(), ratioPassDrawing(), noDupeDrawing() {
    if (!calculate) {
        knnDrawing = image;
        ratioPassDrawing = image;
        noDupeDrawing = image;
        matches = {};
        averageDistance = INFINITY;
        matchingKeypointFraction = -INFINITY;
        return;
    }
    
    BFMatcher matcher(IMProc::Paramater::tileMatcherNormType, IMProc::Paramater::tileMatcherCrossCheck);
    vector<KeyPoint> tileKeypoints;
    Mat tileDescriptors;
    
    IMProc::imageSifter().detect(image, tileKeypoints);
    IMProc::imageSifter().compute(image, tileKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        this->averageDistance = INFINITY;
        this->matches = {};
        
        drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, this->matches, this->knnDrawing);
        drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, this->matches, this->ratioPassDrawing);
        drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, this->matches, this->noDupeDrawing);
        return;
    }
    
    vector<vector<DMatch>> knnMatches;
    matcher.knnMatch(candidate.descriptors, tileDescriptors, knnMatches, 2);
    drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, knnMatches, this->knnDrawing);

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
    
    drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, ratioPassingMatches, this->ratioPassDrawing);
    
    //Remove matches if there is a better match to the same keypoint
    vector<DMatch> noDupeMatches;
    for (auto&& queryMatch : ratioPassingMatches) {
        bool passes = true;
        for (auto&& otherMatch : ratioPassingMatches) {
            if ((queryMatch.queryIdx == otherMatch.queryIdx || queryMatch.trainIdx == otherMatch.trainIdx) && queryMatch.distance < otherMatch.distance) {
                passes = false;
            }
        }
        if (passes) {
            noDupeMatches.push_back(queryMatch);
        }
    }
    drawMatches(candidate.image, candidate.keypoints, image, tileKeypoints, noDupeMatches, this->noDupeDrawing);
    
    this->matches = noDupeMatches;
    if (noDupeMatches.size() == 0) {
        this->averageDistance = INFINITY;
    } else {
        this->averageDistance = accumulate(this->matches.begin(), this->matches.end(), float(0), [](float sum, DMatch d) {
            return sum + d.distance;
        })/this->matches.size();
    }
    this->matchingKeypointFraction = matchNonMatchRatio(candidate.keypoints, tileKeypoints, noDupeMatches);
}

MatchResult IMProc::tileValue(const Mat& tileImage, const map<int, TileInfo>& canonicalTiles) {
    Mat tileDescriptors;
    vector<KeyPoint> tileKeypoints;
    
    IMProc::imageSifter().detect(tileImage, tileKeypoints);
    IMProc::imageSifter().compute(tileImage, tileKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        //Probably either a zero or a 1, guess based on image variance
        Scalar mean;
        Scalar stdDev;
        meanStdDev(tileImage, mean, stdDev);;
        if (stdDev[0] < Paramater::zeroOrOneStdDevThreshold) {
            return MatchResult(TileInfo(tileImage, 0, IMProc::imageSifter()), tileImage, false);
        } else {
            return MatchResult(TileInfo(tileImage, 1, IMProc::imageSifter()), tileImage, false);
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
        if (m.matchingKeypointFraction > Paramater::minimumMatchingKeypointFraction) {
            goodMatchResults.push_back(m);
        }
    }
    
    if (goodMatchResults.empty()) {
        //This is a hack: if the image has descriptors but no good matches, its probably a 1.
        //Alternative would be "return matches[0]" which returns the one with the best average
        //distance among the shitty matches.
        return MatchResult(TileInfo(tileImage, 1, IMProc::imageSifter()), tileImage, false);
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
            result[i+j*4] = IMProc::tileFromIntersection(boardImage, 200*i, 200*j);
        }
    }
    return result;
}

ThreesBoardBase::Board IMProc::boardState(Mat boardImage, const map<int, TileInfo>& canonicalTiles) {
    ThreesBoardBase::Board result;
    array<Mat, 16> images = tileImages(boardImage);
    transform(images.begin(), images.end(), result.begin(), [&canonicalTiles](Mat image){
        return tileValue(image, canonicalTiles).tile.value;
    });
    return result;
}
