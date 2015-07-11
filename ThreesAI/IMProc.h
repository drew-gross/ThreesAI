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
    TileInfo(cv::Mat image, int value, const cv::SIFT& sifter);
    
    cv::Mat image;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    int value;
};

class MatchResult {
public:
    MatchResult(TileInfo t, cv::Mat image, bool calculate=true);
    
    TileInfo tile;
    cv::Mat knnDrawing;
    cv::Mat ratioPassDrawing;
    cv::Mat noDupeDrawing;
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
        
        const int canonicalFeatureCount = 0;
        const int canonicalOctaveLayers = 3;
        const double canonicalContrastThreshold = 0.04; // Higher means more features are rejected for not having enough contrast
        const double canonicalEdgeThreshold = 7; // Higher means less features are rejected for being too edge like
        const double canonicalGaussianSigma = 1;
        
        const int imageFeatureCount = 0;
        const int imageOctaveLayers = 3;
        const double imageContrastThreshold = 0.04; // Higher means more features are rejected for not having enough contrast
        const double imageEdgeThreshold = 10; // Higher means less features are rejected for being too edge like
        const double imageGaussianSigma = 1;
        
        const float goodEnoughAverageMultiplier = 1.4; // Higher means more images are considered candidates to be sorted by number of matching keypoints.
        const float zeroOrOneStdDevThreshold = 4.75; // Lower means more images with no descriptors will be classified as a 1;
        const float minimumMatchingKeypointFraction = 0.065;
        
        const unsigned int ignoredEdgePadding = 40; // Number of pixels to chop off the edge of the each tile image
    }
    
    const cv::Point2f getPoint(const std::string& window);
    const std::array<cv::Point2f, 4> getQuadrilateral(cv::Mat m);
    
    std::vector<cv::Point> findScreenContour(cv::Mat const& image);
    cv::Mat colorImageToBoard(cv::Mat const& colorBoardImage);
    std::array<cv::Mat, 16> tileImages(cv::Mat boardImage);
    ThreesBoardBase::Board boardState(cv::Mat boardImage, const std::map<int, TileInfo>& canonicalTiles);
    MatchResult tileValue(const cv::Mat& tileImage, const std::map<int, TileInfo>& canonicalTiles);
    const cv::Mat tileFromIntersection(cv::Mat image, int x, int y);
    
    const std::map<int, TileInfo>& canonicalTiles();
    const cv::SIFT& canonicalSifter();
    const cv::SIFT& imageSifter();
}

#endif /* defined(__ThreesAI__IMProc__) */
