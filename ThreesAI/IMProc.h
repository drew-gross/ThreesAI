//
//  IMProc.h
//  ThreesAI
//
//  Created by Drew Gross on 6/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__IMProc__
#define __ThreesAI__IMProc__

#include <vector>
#include <array>
#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <boost/optional/optional.hpp>

#include "BoardState.h"

class TileInfo {
public:
    TileInfo();
    TileInfo(cv::Mat image, Tile value, const cv::SIFT& sifter);
    
    cv::Mat image;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    Tile value;
    
    static const TileInfo Tile1Info();
    static const TileInfo Tile2Info();
};

class MatchResult {
public:
    MatchResult();
    MatchResult(TileInfo t, cv::Mat image, bool calculate=true);
    
    cv::Mat knnDrawing();
    cv::Mat ratioPassDrawing();
    cv::Mat noDupeDrawing();
    
    TileInfo tile;
    std::vector<std::vector<cv::DMatch>> knnMatches;
    std::vector<cv::DMatch> ratioPassMatches;
    std::vector<cv::DMatch> noDupeMatches;
    cv::Mat queryImage;
    std::vector<cv::KeyPoint> queryKeypoints;
    float averageDistance;
    float matchingKeypointFraction;
    float quality;
    float imageStdDev;
};

typedef std::map<Tile, TileInfo> CanonicalTiles;

namespace IMProc {
    namespace Paramater {
        const double cannyRejectThreshold = 60;
        const double cannyAcceptThreshold = 100;
        const int cannyApertureSize = 3;
        const bool cannyUseL2 = true;
        
        const float tileMatchRatioTestRatio = 0.75; // Higher means more feature matches are accepted as "good" by the ratio test
        const bool tileMatcherCrossCheck = false;
        const int tileMatcherNormType = cv::NORM_L2;
        
        const int canonicalFeatureCount = 0;
        const int canonicalOctaveLayers = 6;
        const double canonicalContrastThreshold = 0.05; // Higher means more keypoints are rejected for not having enough contrast
        const double canonicalEdgeThreshold = 7; // Higher means less keypoints are rejected for being too edge like
        const double canonicalGaussianSigma = 0.8;
        
        const int imageFeatureCount = 0;
        const int imageOctaveLayers = 6;
        const double imageContrastThreshold = 0.030; // Higher means more keypoints are rejected for not having enough contrast
        const double imageEdgeThreshold = 15; // Higher means less keypoints are rejected for being too edge like
        const double imageGaussianSigma = 1;
        
        const float goodEnoughAverageMultiplier = 1.65; // Higher means more images are considered candidates to be sorted by quality.
        const float matchingKeypointFractionDiscount = -0.01;
        const float zeroOrOneStdDevThreshold = 3.95; // Lower means more images with no descriptors will be classified as a 1.
        const float minimumMatchingKeypointFraction = 0.03;
        const float minimumAverageDistance = 350;
        
        const unsigned int ignoredEdgePadding = 25; // Number of pixels to chop off the edge of the each tile image
        
        const int differenceErosionShape = cv::MORPH_ELLIPSE;
        const cv::Size differenceErosionSize = cv::Size(20,20);
        const float differenceMeanThreshold = 1.5; // Lower means more things that look like six are determined to be not 6.
        
        const float bonusMeanThreshold = 10; //Higher means less next tile hints get interpreted as a bonus tile.
    }
    
    const cv::Point2f getPoint(const std::string& window);
    const std::array<cv::Point2f, 4> getQuadrilateral(cv::Mat m);
    cv::Mat getAveragedImage(cv::VideoCapture& cam, unsigned char numImages);
    
    std::shared_ptr<BoardState const> boardFromAnyImage(cv::Mat const& image, HiddenBoardState otherInfo);
    std::pair<std::shared_ptr<BoardState const>, std::array<MatchResult, 16>> boardAndMatchFromAnyImage(cv::Mat const& image, HiddenBoardState otherInfo);
    std::pair<std::shared_ptr<BoardState const>, std::array<MatchResult, 16>> boardAndMatchFromScreenShot(cv::Mat const& ss, HiddenBoardState otherInfo);
    std::array<cv::Mat, 16> tilesFromScreenImage(cv::Mat const& image);
    cv::Mat screenImage(cv::Mat const& colorBoardImage);
    
    MatchResult tileValue(const cv::Mat& tileImage, const CanonicalTiles& canonicalTiles);
    MatchResult tileValueFromScreenShot(cv::Mat const& image, const CanonicalTiles& canonicalTiles);
    const cv::Mat tileFromIntersection(cv::Mat image, int x, int y);
    
    Hint getHintFromScreenShot(cv::Mat const& ss);
    
    const std::map<Tile, TileInfo>& canonicalTiles();
    const cv::SIFT& canonicalSifter();
    const cv::SIFT& imageSifter();
    
    const std::vector<cv::Mat> color1hints();
    const std::vector<cv::Mat> color2hints();
    const std::vector<cv::Mat> color3hints();
    boost::optional<MatchResult> detect1or2orHigherByColor(cv::Mat const &input);
    boost::optional<MatchResult> detect1or2or3orBonusByColor(cv::Mat const& i);
}

#endif /* defined(__ThreesAI__IMProc__) */
