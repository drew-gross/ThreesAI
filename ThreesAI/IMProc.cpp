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

#include "ForcedHint.hpp"

#include "Debug.h"
#include "Logging.h"
#include "IMLog.h"

using namespace std;
using namespace cv;
using namespace IMLog;

TileInfo::TileInfo(cv::Mat image, int value, const SIFT& sifter) {
    this->image = image;
    this->value = value;
    sifter.detect(image, this->keypoints);
    sifter.compute(image, this->keypoints, this->descriptors);
}

Mat sector(Mat in,
           const float sideFraction,
           const float bottomFraction,
           const float topFraction,
           const Size outputSize) {
    const int width = in.cols;
    const int height = in.rows;
    
    const float leftEdge = width*sideFraction;
    const float rightEdge = width*(1-sideFraction);
    const float bottomEdge = height*bottomFraction;
    const float topEdge = height*topFraction;
    
    const Point2f fromPoints[4] = {
        {leftEdge,topEdge},
        {leftEdge,bottomEdge},
        {rightEdge,bottomEdge},
        {rightEdge,topEdge}
    };
    
    const Point2f toPoints[4] = {
        {0,0},
        {0,static_cast<float>(outputSize.height)},
        {static_cast<float>(outputSize.width),static_cast<float>(outputSize.height)},
        {static_cast<float>(outputSize.width),0}
    };
    
    Mat out;
    warpPerspective(in, out, getPerspectiveTransform(fromPoints, toPoints), outputSize);
    return out;
}

Mat boardImageFromScreen(Mat screenImage) {
    return sector(screenImage, .121, .843, .27, Size(800,800));
}

Mat screenImageToHintImage(Mat const& screenImage) {
    return sector(screenImage, .465, .15, .11, Size(100,100));
}

Mat screenImageToBonusHintImage(Mat const& screenImage) {
    return sector(screenImage, .35, .15, .11, Size(300,100));
}

vector<Point> findScreenContour(Mat const& image) {
    
    Mat blurredCopy;
    GaussianBlur(image, blurredCopy, Size(5,5), 2);
    
    Mat cannyCopy;
    Canny(blurredCopy, cannyCopy, IMProc::Paramater::cannyRejectThreshold, IMProc::Paramater::cannyAcceptThreshold, IMProc::Paramater::cannyApertureSize, IMProc::Paramater::cannyUseL2);
    
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

Mat IMProc::screenImage(Mat const& colorBoardImage) {
    Mat greyBoardImage;
    Mat screenImage;
    
    cvtColor(colorBoardImage, greyBoardImage, CV_RGB2GRAY);
    
    vector<Point> screenContour = findScreenContour(greyBoardImage);
    
    //TODO: handle empty screenContour
    const Point2f fromCameraPoints[4] = {screenContour[0], screenContour[1], screenContour[2], screenContour[3]};
    
    
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    warpPerspective(colorBoardImage, screenImage, getPerspectiveTransform(fromCameraPoints, toPoints), Size(800,800));
    
    return screenImage;
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

Mat IMProc::getAveragedImage(VideoCapture& cam, unsigned char numImages) {
    vector<Mat> images;
    while (images.size() < 8) {
        Mat image;
        cam >> image;
        if (image.rows != 0 && image.cols != 0) {
            images.push_back(image);
        }
    }
    Mat averagedImage;
    if (numImages == 0) {
        return averagedImage;
    }
    averagedImage = Mat::zeros(images[0].rows, images[0].cols, images[0].type());
    for (auto&& image : images) {
        debug(image.rows != averagedImage.rows || image.cols != averagedImage.cols);
        averagedImage += image/numImages;
    }
    return averagedImage;
}

const int L12 = 80;
const int R12 = 200;
const int T12 = 310;
const int B12 = 630;
const Point2f fromPoints12[4] = {{L12,T12},{L12,B12},{R12,B12},{R12,T12}};
const Point2f toPoints12[4] = {{0,0},{0,400},{200,400},{200,0}};

Rect flatRegionRect = Rect(0,0,40,100);

const vector<Mat> IMProc::color1hints() {
    static Mat image1 = screenImageToHintImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Hint1.png")));
    static Mat image2 = screenImageToHintImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Hint2.png")));
    static Mat image3 = screenImageToHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Hint3.png"));
    static Mat image4 = tilesFromScreenImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Tile7.png")))[7](flatRegionRect);
    static Mat image5 = tilesFromScreenImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Tile9.png")))[9](flatRegionRect);
    static Mat image6 = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample1Only.png");
    return {image1, image2, image3, image4, image5, image6};
}

const vector<Mat> IMProc::color2hints() {
    static Mat nonHintImage = imread("/Users/drewgross/Projects/ThreesAI/SampleData/1,2,1,6,6,24,1,6,1,48,96,2,3,2,12,6.png");
    static array<Mat, 16> tiles = tilesFromScreenImage(screenImage(nonHintImage));
    static Mat image1 = screenImageToHintImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample2Hint1.png")));
    static Mat image2 = tilesFromScreenImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample2Tile12.png")))[12](flatRegionRect);
    static Mat image3 = tilesFromScreenImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample2Tile12-1.png")))[12](flatRegionRect);
    static Mat image4 = tilesFromScreenImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample2Tile13.png")))[13](flatRegionRect);
    return {tiles[1](flatRegionRect), image1, image2, image3, image4};
    
}

const vector<Mat> IMProc::color3hints() {
    static Mat image1 = screenImageToHintImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample3Hint1.png")));
    static Mat image2 = screenImageToHintImage(screenImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample3Hint2.png")));
    static Mat image3 = screenImageToHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Sample3Hint3.png"));
    return {image1, image2, image3};
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

TileInfo::TileInfo(){};

MatchResult::MatchResult() : tile() {};

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
        this->quality = this->averageDistance/(
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
    
    bool no6 = !(find_if(matches.begin(), end_it, [](MatchResult m){
        return m.tile.value == 6;
    }) != end_it);
    bool no96 = !(find_if(matches.begin(), end_it, [](MatchResult m){
        return m.tile.value == 96;
    }) != end_it);
    bool no192 = !(find_if(matches.begin(), end_it, [](MatchResult m){
        return m.tile.value == 192;
    }) != end_it);
    
    if ((no6 && no96) || (no6 && no192) || (no96 && no192)) {
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
        (matches[0].matchingKeypointFraction * 0.6 > matches[1].matchingKeypointFraction)) {
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
        
        Mat lBinary;
        Mat rBinary;
        threshold(l.tile.image, lBinary, 0, 255, THRESH_OTSU);
        threshold(r.tile.image, rBinary, 0, 255, THRESH_OTSU);
        bitwise_xor(tileEroded, lBinary, differenceL);
        bitwise_xor(tileEroded, rBinary, differenceR);
        
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
        Rect chopSides(10,10,140,140);
        meanStdDev(greyTileImage(chopSides), mean, stdDev);
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

array<Mat, 16> tileImages(Mat boardImage) {
    array<Mat, 16> result;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            result[i+j*4] = IMProc::tileFromIntersection(boardImage, 200*i, 200*j);
        }
    }
    return result;
}

double distanceToNearestInVector(Mat query, vector<Mat> train) {
    double closestDist = INFINITY;
    Scalar queryMean = mean(query);
    for (Mat sample : train) {
        double newDistance = norm(queryMean, mean(sample));
        if (newDistance < closestDist) {
            closestDist = newDistance;
        }
    }
    return closestDist;
}

unsigned int IMProc::detect1or2orHigherByColor(Mat const &input) {
    double closestSample1distance = distanceToNearestInVector(input, color1hints());
    double closestSample2distance = distanceToNearestInVector(input, color2hints());
    double closestSample3distance = distanceToNearestInVector(input, color3hints());
    
    if (closestSample1distance < closestSample2distance && closestSample1distance < closestSample3distance) {
        return 1;
    } else if (closestSample2distance < closestSample3distance) {
        return 2;
    } else {
        return 3;
    }
}

unsigned int IMProc::detect1or2or3orBonusByColor(Mat const &input) {
    Scalar inputMean;
    Scalar iStdDev;
    meanStdDev(input, inputMean, iStdDev);
    
    Scalar m = mean(iStdDev);
    if (m[0] > Paramater::bonusMeanThreshold) {
        return 4;
    } else {
        return detect1or2orHigherByColor(input);
    }
}

array<Mat, 16> IMProc::tilesFromScreenImage(Mat const& image) {
    return tileImages(boardImageFromScreen(image));
}

float minShiftedMean(Mat const& query, Mat const& train) {
    Mat t(2,3,CV_32F);
    
    t.at<float>(0,0) = 1;
    t.at<float>(0,1) = 0;
    t.at<float>(0,2) = 0;
    
    t.at<float>(1,0) = 0;
    t.at<float>(1,1) = 1;
    t.at<float>(1,2) = 0;
    
    float minMean = INFINITY;
    for (int i = -5; i <= 5; i += 5) {
        for (int j = -5; j <= 5; j += 5) {
            t.at<float>(0,2) = i;
            t.at<float>(1,2) = j;
            Mat shiftedQuery;
            warpAffine(query, shiftedQuery, t, Size(query.cols, query.rows), INTER_LINEAR, BORDER_REPLICATE);
            Mat diff;
            absdiff(shiftedQuery, train, diff);
            //MYSHOW(diff);\
            MYSHOW(shiftedQuery);\
            MYSHOW(train);
            
            minMean = MIN(minMean, mean(diff)[0]);
        }
    }
    return minMean;
}

MatchResult IMProc::tileValueFromScreenShot(Mat const& tileSS, map<int, TileInfo> const& canonicalTiles) {
    Scalar mean;
    Scalar stdDev;
    meanStdDev(tileSS(Rect(50,50,100,100)), mean, stdDev);
    if (stdDev[0] <= 1) {
        return MatchResult(canonicalTiles.at(0), tileSS);
    }
    unsigned int colorDetectionResult = IMProc::detect1or2orHigherByColor(tileSS);
    if (colorDetectionResult < 3) {
        return MatchResult(canonicalTiles.at(colorDetectionResult), tileSS);
    }
    Mat greyTile;
    cvtColor(tileSS, greyTile, CV_RGB2GRAY);
    
    Mat binaryTileSS;
    threshold(greyTile, binaryTileSS, 200, 255, THRESH_BINARY);
    
    pair<int, TileInfo> bestMatch = *min_element(canonicalTiles.begin(), canonicalTiles.end(), [&binaryTileSS](pair<int, TileInfo> l, pair<int, TileInfo> r){
        Mat binaryCanonicalTileL;
        Mat binaryCanonicalTileR;
        threshold(l.second.image, binaryCanonicalTileL, 200, 255, THRESH_BINARY);
        threshold(r.second.image, binaryCanonicalTileR, 200, 255, THRESH_BINARY);
        
        return minShiftedMean(binaryTileSS, binaryCanonicalTileL) < minShiftedMean(binaryTileSS, binaryCanonicalTileR);
    });
    if (bestMatch.second.value == 768 || bestMatch.second.value == 384) {
        MatchResult SIFTresult = IMProc::tileValue(tileSS, canonicalTiles);
        //768 and 384 get confused less often in the SIFT scheme.
        if (SIFTresult.tile.value == 768 || SIFTresult.tile.value == 384) {
            return SIFTresult;
        }
    }
    return MatchResult(bestMatch.second, tileSS);
}

shared_ptr<Hint const> IMProc::getHintFromScreenShot(Mat const& ss) {
    static vector<pair<shared_ptr<ForcedHint const>, Mat>> hintImages({
        {make_shared<ForcedHint const>(12,24,48), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-12-24-48.png", 0))},
        {make_shared<ForcedHint const>(6,12,24), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12-24.png", 0))},
        {make_shared<ForcedHint const>(6,12), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12.png", 0))},
        {make_shared<ForcedHint const>(6), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6.png", 0))},
        
    });
    Mat narrowHint = screenImageToHintImage(ss);
    unsigned int narrowHintResult = detect1or2or3orBonusByColor(narrowHint);
    if (narrowHintResult <= 3) {
        return make_shared<ForcedHint const>(narrowHintResult);
    }
    
    Mat greyHint;
    cvtColor(screenImageToBonusHintImage(ss), greyHint, CV_RGB2GRAY);
    
    Mat binaryHintSS;
    threshold(greyHint, binaryHintSS, 200, 255, THRESH_BINARY);
    
    pair<shared_ptr<ForcedHint const>, Mat> bestMatch = *min_element(hintImages.begin(), hintImages.end(), [&binaryHintSS](pair<shared_ptr<ForcedHint const>, Mat> l, pair<shared_ptr<ForcedHint const>, Mat> r){
        
        Mat binaryCanonicalTileL;
        Mat binaryCanonicalTileR;
        threshold(l.second, binaryCanonicalTileL, 1, 255, THRESH_OTSU);
        threshold(r.second, binaryCanonicalTileR, 1, 255, THRESH_OTSU);
        //MYSHOW(binaryHintSS);\
        MYSHOW(binaryCanonicalTileL);\
        MYSHOW(binaryCanonicalTileR);
        
        return minShiftedMean(binaryHintSS, binaryCanonicalTileL) < minShiftedMean(binaryHintSS, binaryCanonicalTileR);
    });
    return bestMatch.first;
}

pair<BoardState, array<MatchResult, 16>> IMProc::boardAndMatchFromScreenShot(Mat const& ss) {
    shared_ptr<Hint const> hint = getHintFromScreenShot(ss);
    auto tileImages = tilesFromScreenImage(ss);
    const map<int, TileInfo>& canonicalTiles = IMProc::canonicalTiles();
    array<MatchResult, 16> matches;
    
    transform(tileImages.begin(), tileImages.end(), matches.begin(), [&canonicalTiles](Mat image){
        return tileValueFromScreenShot(image, canonicalTiles);
    });
    
    BoardState::Board board;
    transform(matches.begin(), matches.end(), board.begin(), [](MatchResult m) {
        return m.tile.value;
    });
    
    return {BoardState(board, hint, 0, ss, 4, 4, 4), matches};
}

pair<BoardState, array<MatchResult, 16>> IMProc::boardAndMatchFromAnyImage(Mat const& image) {
    if (image.rows == 2272 && image.cols == 1280) {
        return boardAndMatchFromScreenShot(image);
    }
    
    array<Mat, 16> images = IMProc::tilesFromScreenImage(screenImage(image));
    const map<int, TileInfo>& canonicalTiles = IMProc::canonicalTiles();
    array<MatchResult, 16> matches;
    transform(images.begin(), images.end(), matches.begin(), [&canonicalTiles](Mat image){
        return IMProc::tileValue(image, canonicalTiles);
    });
    
    BoardState::Board board;
    transform(matches.begin(), matches.end(), board.begin(), [](MatchResult m) {
        return m.tile.value;
    });
    
    unsigned int hint = IMProc::detect1or2or3orBonusByColor(screenImageToHintImage(screenImage(image)));
    if (hint < 4) {
        return {BoardState(board, make_shared<ForcedHint const>(hint), 0, image, 4, 4, 4), matches};
    } else {
        //TODO: get the real bonus tile hint here
        return {BoardState(board, ForcedHint::unknownBonus(), 0, image, 4, 4, 4), matches};
    }
}

BoardState IMProc::boardFromAnyImage(Mat const& image) {
    return boardAndMatchFromAnyImage(image).first;
}

