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
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "Debug.h"
#include "Logging.h"
#include "IMLog.h"

using namespace std;
using namespace cv;
using namespace IMLog;

Mat screenImageToHintImage(Mat i) {
    const float left = 375;
    const float right = 425;
    const float top = 80;
    const float bottom = 120;
    const float outputWidth = 100;
    const float outputHeight = 100;
    const Point2f fromPointsHint[4] = {{left,top},{left,bottom},{right,bottom},{right,top}};
    const Point2f toPointsHint[4] = {{0,0},{0,outputHeight},{outputWidth,outputHeight},{outputWidth,0}};
    Mat hintImage;
    warpPerspective(i, hintImage, getPerspectiveTransform(fromPointsHint, toPointsHint), Size(outputWidth,outputHeight));
    return hintImage;
}

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

const int L12 = 80;
const int R12 = 200;
const int T12 = 310;
const int B12 = 630;
const Point2f fromPoints12[4] = {{L12,T12},{L12,B12},{R12,B12},{R12,T12}};
const Point2f toPoints12[4] = {{0,0},{0,400},{200,400},{200,0}};

const Mat IMProc::color1sample() {
    static Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Hint.png");
    static array<Mat, 16> tiles = IMProc::tileImages(IMProc::boardImageFromScreen(IMProc::screenImage(image)));
    return tiles[0];
}

const vector<Mat> IMProc::color1hints() {
    static Mat image1 = screenImageToHintImage(IMProc::screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Hint1.png")));
    static Mat image2 = screenImageToHintImage(IMProc::screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Hint2.png")));
    return {image1, image2};
}

const Mat IMProc::color2sample() {
    static Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/1,2,1,6,6,24,1,6,1,48,96,2,3,2,12,6.png");
    static array<Mat, 16> tiles = IMProc::tileImages(IMProc::boardImageFromScreen(IMProc::screenImage(image)));
    return tiles[1];
}

const vector<Mat> IMProc::color3hints() {
    static Mat image1 = screenImageToHintImage(IMProc::screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample3Hint1.png")));
    static Mat image2 = screenImageToHintImage(IMProc::screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample3Hint2.png")));
    return {image1, image2};
}

const Mat IMProc::color12(int which) {
    static bool hasBoard = false;
    static Mat board12;
    if (hasBoard) {
        return tileFromIntersection(board12, 0, which == 1 ? 200 : 0);
    }
    Mat image12 = imread("/Users/drewgross/Projects/ThreesAI/SampleData/12.png");

    
    warpPerspective(image12, board12, getPerspectiveTransform(fromPoints12, toPoints12), Size(200,400));
    hasBoard = true;
    return color12(which);
}

const map<int, TileInfo>* loadCanonicalTiles() {
    Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Tiles.png", 0);
    Mat boardImage;
    
    map<int, TileInfo>* results = new map<int, TileInfo>();
    
    Mat grey1;
    Mat grey2;
    cvtColor(IMProc::color12(1), grey1, COLOR_RGB2GRAY);
    cvtColor(IMProc::color12(2), grey2, COLOR_RGB2GRAY);
    results->emplace(piecewise_construct, forward_as_tuple(1), forward_as_tuple(grey1, 1, IMProc::canonicalSifter()));
    results->emplace(piecewise_construct, forward_as_tuple(2), forward_as_tuple(grey2, 2, IMProc::canonicalSifter()));
    
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

const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};

Mat IMProc::screenImage(Mat const& colorBoardImage) {
    Mat greyBoardImage;
    Mat screenImage;
    
    cvtColor(colorBoardImage, greyBoardImage, CV_RGB2GRAY);
    
    vector<Point> screenContour = IMProc::findScreenContour(greyBoardImage);
    
    //TODO: handle empty screenContour
    const Point2f fromCameraPoints[4] = {screenContour[0], screenContour[1], screenContour[2], screenContour[3]};
    
    warpPerspective(colorBoardImage, screenImage, getPerspectiveTransform(fromCameraPoints, toPoints), Size(800,800));
    
    return screenImage;
}

Mat IMProc::boardImageFromScreen(Mat screenImage) {
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
    Mat boardImage;
    warpPerspective(screenImage, boardImage, getPerspectiveTransform(fromScreenPoints, toPoints), Size(800,800));
    return boardImage;
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

MatchResult::MatchResult(TileInfo candidate, Mat colorImage, bool calculate) : tile(candidate), queryImage(), queryKeypoints(), knnMatches(), ratioPassMatches(), noDupeMatches(), averageDistance(INFINITY), matchingKeypointFraction(-INFINITY) {
    
    cvtColor(colorImage, this->queryImage, CV_RGB2GRAY);
    
    Scalar mean;
    Scalar stdDev;
    meanStdDev(this->queryImage, mean, stdDev);
    
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
    
    this->matchingKeypointFraction = float(noDupeMatches.size())/(this->tile.keypoints.size() + this->queryKeypoints.size());
    
    if (this->matchingKeypointFraction > -IMProc::Paramater::matchingKeypointFractionDiscount) {
        this->quality = (
        this->averageDistance)/(
        (this->matchingKeypointFraction + IMProc::Paramater::matchingKeypointFractionDiscount)*
        pow(this->noDupeMatches.size(), 0.8));
    } else {
        this->quality = INFINITY;
    }
}

Mat getHistogram(Mat i) {
    Mat hsv;
    cvtColor(i, hsv, COLOR_RGB2HSV);
    int channels[] = { 0, 1 };
    Mat hist;
    
    /// Using 50 bins for hue and 60 for saturation
    int h_bins = 50; int s_bins = 60;
    int histSize[] = { h_bins, s_bins };
    // hue varies from 0 to 179, saturation from 0 to 255
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    
    const float* ranges[] = { h_ranges, s_ranges };
    calcHist(&i, 1, channels, Mat(), hist, 2, histSize, ranges);
    return hist;
}

bool mustDetect6vs96vs192(deque<MatchResult> matches) {
    size_t numElemsToCheck = min(matches.size(), (size_t)3);
    
    auto end_it = matches.begin()+numElemsToCheck;
    
    bool has6 = find_if(matches.begin(), end_it, [](MatchResult m){
        return m.tile.value == 6;
    }) != end_it;
    bool has96 = find_if(matches.begin(), end_it, [](MatchResult m){
        return m.tile.value == 96;
    }) != end_it;
    bool has192 = find_if(matches.begin(), end_it, [](MatchResult m){
        return m.tile.value == 192;
    }) != end_it;
    
    if ((!has6 && !has96) || (!has6 && !has192) || (!has96 && !has192)) {
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

MatchResult detect6vs96vs192(deque<MatchResult> matches, Mat greyTileImage) {
    Mat tileBinary;
    Mat tileEroded;
    threshold(greyTileImage, tileBinary, 0, 255, THRESH_OTSU);
    morphologyEx(tileBinary, tileEroded, MORPH_ERODE, getStructuringElement(IMProc::Paramater::differenceErosionShape, IMProc::Paramater::differenceErosionSize));
    
    size_t numElemsToCheck = min(matches.size(), (size_t)3);
    return *min_element(matches.begin(), matches.begin()+numElemsToCheck, [&tileEroded](MatchResult l, MatchResult r){
        Mat differenceL;
        Mat differenceR;
        subtract(tileEroded, l.tile.image, differenceL);
        subtract(tileEroded, r.tile.image, differenceR);
        
        Mat numeralsOnlyL = differenceL(Rect(0, 0, differenceL.cols, differenceL.rows-30));
        Mat numeralsOnlyR = differenceR(Rect(0, 0, differenceR.cols, differenceR.rows-30));
        
        Scalar meanL;
        Scalar meanR;
        Scalar stdDevL;
        Scalar stdDevR;
        
        meanStdDev(numeralsOnlyL, meanL, stdDevL);
        meanStdDev(numeralsOnlyR, meanR, stdDevR);
        
        return meanL[0] < meanR[0];
    });
}

MatchResult IMProc::tileValue(const Mat& colorTileImage, const map<int, TileInfo>& canonicalTiles) {
    Mat greyTileImage;
    cvtColor(colorTileImage, greyTileImage, CV_RGB2GRAY);
    Mat tileDescriptors;
    vector<KeyPoint> tileKeypoints;
    
    IMProc::imageSifter().detect(greyTileImage, tileKeypoints);
    IMProc::imageSifter().compute(greyTileImage, tileKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        //Probably either a zero or a 1, guess based on image variance
        Scalar mean;
        Scalar stdDev;
        meanStdDev(greyTileImage, mean, stdDev);;
        if (stdDev[0] < Paramater::zeroOrOneStdDevThreshold) {
            return MatchResult(TileInfo(canonicalTiles.at(0).image, 0, IMProc::imageSifter()), colorTileImage, false);
        } else {
            return MatchResult(TileInfo(canonicalTiles.at(1).image, 1, IMProc::imageSifter()), colorTileImage, false);
        }
    }
    
    vector<MatchResult> matchResults;
    for (auto&& t : canonicalTiles) {
        matchResults.emplace_back(t.second, colorTileImage);
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
    
    if (mustDetect6vs96vs192(goodMatchResults)) {
        return detect6vs96vs192(goodMatchResults, greyTileImage);
    }
    
    if (goodMatchResults[0].tile.value == 1 ||
        goodMatchResults[0].tile.value == 2)
    {
        Rect flatRegionRect = Rect(0,0,40,100);
        int result = detect1or2or3orBonusByColor(colorTileImage(flatRegionRect));
        for (auto&& m : matchResults) {
            if (m.tile.value == result) {
                return m;
            }
        }
    }
    
    // Tiles can sometimes masquarade as 1s even with a terrible average distance,
    // because there are few keypoints so they can have a super high matching keypoint
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

unsigned int IMProc::detect1or2or3orBonusByColor(Mat input) {
    Rect flatRegionRect = Rect(0,0,40,100);
    Mat i2 = IMProc::color2sample();
    Scalar inputMean;
    Scalar iStdDev;
    meanStdDev(input, inputMean, iStdDev);
    
    
    Scalar i2Mean;
    Scalar i2StdDev;
    
    Scalar i3Mean;
    Scalar i3StdDev;
    
    double closestSample1distance = INFINITY;
    Mat closestSample1Mat;
    for (auto&& sample : color1hints()) {
        Scalar sampleMean = mean(sample);
        double newDistance = norm(inputMean, sampleMean);
        if (newDistance < closestSample1distance) {
            closestSample1distance = newDistance;
            closestSample1Mat = sample;
        }
    }
    
    double closestSample3distance = INFINITY;
    Mat closestSample3Mat;
    for (auto&& sample : color3hints()) {
        Scalar sampleMean = mean(sample);
        double newDistance = norm(inputMean, sampleMean);
        if (newDistance < closestSample3distance) {
            closestSample3distance = newDistance;
            closestSample3Mat = sample;
        }
    }
    
    Scalar m = mean(iStdDev);
    if (m[0] > 4) {
        return 4; //Bonus
    }
    double d2 = norm(inputMean, mean(i2(flatRegionRect)));
    if (closestSample1distance < d2 && closestSample1distance < closestSample3distance) {
        return 1;
    } else if (d2 < closestSample3distance) {
        return 2;
    } else {
        return 3;
    }
}

IMProc::BoardInfo IMProc::boardState(Mat screenImage, const map<int, TileInfo>& canonicalTiles) {
    ThreesBoardBase::Board board;
    array<Mat, 16> images = tileImages(boardImageFromScreen(screenImage));
    transform(images.begin(), images.end(), board.begin(), [&canonicalTiles](Mat image){
        return tileValue(image, canonicalTiles).tile.value;
    });

    unsigned int hint = IMProc::detect1or2or3orBonusByColor(screenImageToHintImage(screenImage));
    if (hint < 4) {
        return {board, {hint}};
    } else {
        //TODO: get the real bonus tile hint here
        return {board, {6,12,24,48,96,192,384,768,1536}};
    }
}

