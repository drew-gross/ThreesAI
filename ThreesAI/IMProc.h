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

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "ThreesBoardBase.h"

class TileInfo {
public:
    TileInfo(cv::Mat image, int value);
    
    cv::Mat image;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    int value;
};

class MatchResult {
public:
    MatchResult(TileInfo matchedTile, cv::Mat matchDrawing, std::vector<cv::DMatch> matches, float averageDistance, float matchingKeypointsFraction);
    MatchResult(TileInfo t, cv::Mat image);
    
    TileInfo tile;
    cv::Mat drawing;
    std::vector<cv::DMatch> matches;
    float averageDistance;
    float matchingKeypointFraction;
};

namespace IMProc {
    namespace Paramater {
        const double cannyRejectThreshold = 60;
        const double cannyAcceptThreshold = 100;
        const int cannyApertureSize = 3;
        const bool cannyUseL2 = true;
        
        const float tileMatchRatioTestRatio = 0.75; // Higher means more feature matches are accepted as "good" by the ratio test
        const bool tileMatcherCrossCheck = false;
        const int tileMatcherNormType = cv::NORM_L2;
        
        const int siftFeatureCount = 0;
        const int siftOctaveLayers = 3;
        const double siftContrastThreshold = 0.04; // Higher means more features are rejected for not having enough contrast
        const double siftEdgeThreshold = 10; // Higher means less features are rejected for being too edge like
        const double siftGaussianSigma = 1;
        
        const float goodEnoughAverageMultiplier = 1.4;
        const float zeroOrOneStdDevThreshold = 4.5; // Lower means more images with no descriptors will be classified as a 1;
        const float minimumMatchingKeypointFraction = 0.055;
    }
    
    const cv::Point2f getPoint(const std::string& window);
    const std::array<cv::Point2f, 4> getQuadrilateral(cv::Mat m);
    
    std::vector<cv::Point> findScreenContour(cv::Mat const& image);
    cv::Mat colorImageToBoard(cv::Mat const& colorBoardImage);
    std::array<cv::Mat, 16> tileImages(cv::Mat boardImage);
    ThreesBoardBase::Board boardState(cv::Mat boardImage, const std::map<int, TileInfo>& canonicalTiles);
    MatchResult tileValue(const cv::Mat& tileImage, const std::map<int, TileInfo>& canonicalTiles);
    
    const std::map<int, TileInfo>& canonicalTiles();
    const cv::SIFT& sifter();
}

#endif /* defined(__ThreesAI__IMProc__) */
