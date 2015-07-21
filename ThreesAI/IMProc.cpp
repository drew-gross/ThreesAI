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

Mat IMProc::concatH(vector<Mat> v) {
    int totalWidth = accumulate(v.begin(), v.end(), 0, [](int s, Mat m){
        return s + m.cols;
    });
    int maxHeight = std::max_element(v.begin(), v.end(), [](Mat first, Mat second){
        return first.rows < second.rows;
    })->rows;
    
    Mat combined(maxHeight, totalWidth, v[0].type());
    
    int widthSoFar = 0;
    for (auto it = v.begin(); it != v.end(); it++) {
        it->copyTo(combined(Rect(widthSoFar, 0, it->cols, it->rows)));
        widthSoFar += it->cols;
    }
    
    return combined;
}

Mat IMProc::concatV(vector<Mat> v) {
    int totalHeight = accumulate(v.begin(), v.end(), 0, [](int s, Mat m){
        return s + m.rows;
    });
    int maxWidth = std::max_element(v.begin(), v.end(), [](Mat first, Mat second){
        return first.cols < second.cols;
    })->cols;
    
    Mat combined(totalHeight, maxWidth, v[0].type());
    
    int heightSoFar = 0;
    for (auto it = v.begin(); it != v.end(); it++) {
        it->copyTo(combined(Rect(0, heightSoFar, it->cols, it->rows)));
        heightSoFar += it->rows;
    }
    
    return combined;
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

void IMProc::showContours(Mat const image, vector<vector<Point>> const contours) {
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

Mat MatchResult::knnDrawing() {
    Mat drawing;
    drawMatches(this->tile.image, this->tile.keypoints, this->queryImage, queryKeypoints, this->knnMatches, drawing);
    return drawing;
}

Mat MatchResult::ratioPassDrawing() {
    Mat drawing;
    drawMatches(this->tile.image, this->tile.keypoints, this->queryImage, queryKeypoints, this->ratioPassMatches, drawing);
    return drawing;
}

Mat MatchResult::noDupeDrawing() {
    Mat drawing;
    drawMatches(this->tile.image, this->tile.keypoints, this->queryImage, queryKeypoints, this->noDupeMatches, drawing);
    return drawing;
}

MatchResult::MatchResult(TileInfo candidate, Mat queryImage, bool calculate) : tile(candidate), queryImage(queryImage), queryKeypoints(), knnMatches(), ratioPassMatches(), noDupeMatches(), averageDistance(INFINITY), matchingKeypointFraction(-INFINITY) {
    Scalar mean;
    Scalar stdDev;
    meanStdDev(queryImage, mean, stdDev);
    
    this->imageStdDev = stdDev[0];
    
    if (!calculate) {
        return;
    }
    
    BFMatcher matcher(IMProc::Paramater::tileMatcherNormType, IMProc::Paramater::tileMatcherCrossCheck);
    Mat tileDescriptors;
    
    IMProc::imageSifter().detect(this->queryImage, this->queryKeypoints);
    IMProc::imageSifter().compute(this->queryImage, this->queryKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        return;
    }
    
    matcher.knnMatch(candidate.descriptors, tileDescriptors, this->knnMatches, 2);

    //Ratio test
    for (int i = 0; i < knnMatches.size(); ++i) {
        if (knnMatches[i].size() > 1) {
            if (knnMatches[i][0].distance < IMProc::Paramater::tileMatchRatioTestRatio * knnMatches[i][1].distance) {
                this->ratioPassMatches.push_back(knnMatches[i][0]);
            }
        } else if (knnMatches[i].size() == 1) {
            this->ratioPassMatches.push_back(knnMatches[i][0]);
        }
    }
    
    //Remove matches if there is a better match to the same keypoint, because there cannot be 2 matches to the same keypoint.
    for (auto&& queryMatch : this->ratioPassMatches) {
        bool passes = true;
        for (auto&& otherMatch : this->ratioPassMatches) {
            if (candidate.value == 96 && queryMatch.trainIdx == otherMatch.trainIdx && queryMatch.distance < otherMatch.distance) {
                //96 might legitimately have 2 matches from candidate tile to photo because 9 looks like 6.
                passes = false;
            } else if ((queryMatch.queryIdx == otherMatch.queryIdx || queryMatch.trainIdx == otherMatch.trainIdx) && queryMatch.distance < otherMatch.distance) {
                passes = false;
            }
        }
        if (passes) {
            this->noDupeMatches.push_back(queryMatch);
        }
    }
    
    if (this->noDupeMatches.size() == 0) {
        this->averageDistance = INFINITY;
    } else {
        this->averageDistance = accumulate(this->noDupeMatches.begin(), this->noDupeMatches.end(), float(0), [](float sum, DMatch d) {
            return sum + d.distance;
        })/this->noDupeMatches.size();
    }
    
    this->matchingKeypointFraction = float(noDupeMatches.size())/(pow(this->tile.keypoints.size(), 1.3) + this->queryKeypoints.size());
    
    if (this->matchingKeypointFraction > -IMProc::Paramater::matchingKeypointFractionDiscount) {
        this->quality = this->averageDistance/(this->matchingKeypointFraction + IMProc::Paramater::matchingKeypointFractionDiscount);
    } else {
        this->quality = INFINITY;
    }
}

bool mustDetect6vs96(deque<MatchResult> matches) {
    if (matches.size() < 2) {
        return false;
    }
    bool hasRightTiles = (matches[0].tile.value == 6 && matches[1].tile.value == 96) ||
    (matches[1].tile.value == 6 && matches[0].tile.value == 96);
    
    if (!hasRightTiles) {
        return false;
    }
    
    // Make sure there is actually significant ambiguity
    bool ambiguousAverage = matches[0].averageDistance > matches[1].averageDistance;
    bool ambiguousFraction = matches[0].matchingKeypointFraction < matches[1].matchingKeypointFraction;
    
    // If keypoints fraction and average distance disagree, definitely need to diff
    if (ambiguousAverage || ambiguousFraction) {
        return true;
    }
    
    // If keypoints fraction and average distance agree, and aren't even close
    // to disagreeing, don't need to diff
    if ((matches[0].averageDistance * 1.3 < matches[1].averageDistance) &&
        (matches[0].matchingKeypointFraction * 0.8 > matches[1].matchingKeypointFraction)) {
        return false;
    }
    
    return true;
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
            return MatchResult(TileInfo(canonicalTiles.at(0).image, 0, IMProc::imageSifter()), tileImage, false);
        } else {
            return MatchResult(TileInfo(canonicalTiles.at(1).image, 1, IMProc::imageSifter()), tileImage, false);
        }
    }
    
    vector<MatchResult> matchResults;
    for (auto&& t : canonicalTiles) {
        matchResults.emplace_back(t.second, tileImage);
    }
    
    sort(matchResults.begin(), matchResults.end(), [](MatchResult l, MatchResult r){
        //If all the matches were eliminated by ratio test, use number of total matches
        if (l.averageDistance == INFINITY && r.averageDistance == INFINITY) {
            return l.noDupeMatches.size() > r.noDupeMatches.size();
        }
        
        return l.quality < r.quality;
    });
    
    deque<MatchResult> goodMatchResults;
    for (auto&& m : matchResults) {
        if (m.matchingKeypointFraction > Paramater::minimumMatchingKeypointFraction) {
            goodMatchResults.push_back(m);
        }
    }
    
    if (goodMatchResults.empty()) {
        // If none of the matches are good, use the best of the shitty ones. But not tile 0.
        if (matchResults[0].tile.value == 0) {
            return matchResults[1];
        } else {
            return matchResults[0];
        }
    }
    
    if (mustDetect6vs96(goodMatchResults)) {
        //Detect by diffing with canonical image
        Mat difference;
        Mat tileBinary;
        Mat tileEroded;
        threshold(tileImage, tileBinary, 0, 255, THRESH_OTSU);
        morphologyEx(tileBinary, tileEroded, MORPH_ERODE, getStructuringElement(Paramater::differenceErosionShape, Paramater::differenceErosionSize));
        subtract(tileEroded, goodMatchResults[0].tile.image, difference);
        Mat numeralsOnly = difference(Rect(0, 0, tileImage.cols, tileImage.rows-30));
        Scalar mean;
        Scalar stdDev;
        meanStdDev(numeralsOnly, mean, stdDev);
        //MYSHOW(Log::concatV({tileImage, difference, tileBinary, tileEroded, goodMatchResults[0].tile.image, numeralsOnly})); debug();
        if (mean[0] > Paramater::differenceMeanThreshold) {
            goodMatchResults.pop_front();
        }
    }
    
    // Tiles can sometimes masquarade as 1s even with a terrible average distance,
    // because there is only 1 keypoint so they can have a super high matching keypoint
    // fraction. So get rid of the first match if it has a terrible distance and
    // there is a second match.
    if (goodMatchResults.size() > 1 && goodMatchResults[0].averageDistance > Paramater::minimumAverageDistance && goodMatchResults[0].averageDistance > goodMatchResults[1].averageDistance) {
        return goodMatchResults[1];
    } else {
        return goodMatchResults[0];
    }
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

