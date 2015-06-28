//
//  IMProc.h
//  ThreesAI
//
//  Created by Drew Gross on 6/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__IMProc__
#define __ThreesAI__IMProc__

#include <stdio.h>

#include <vector>
#include <array>

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

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
    MatchResult(TileInfo matchedTile, cv::Mat matchDrawing, std::vector<cv::DMatch> matches);
    
    TileInfo matchedTile;
    cv::Mat matchDrawing;
    std::vector<cv::DMatch> matches;
};

namespace IMProc {
    namespace Paramater {
        const float tileMatchRatioTestRatio = 0.6;
        const bool tileMatcherCrossCheck = false;
        const int tileMatcherNormType = cv::NORM_L2;
        
        const int siftFeatureCount = 0;
        const int siftOctaveLayers = 3;
        const double siftContrastThreshold = 0.04;
        const double siftEdgeThreshold = 10;
        const double siftGaussianSigma = 1;
    }
    
    std::vector<cv::Point> findScreenContour(cv::Mat const& image);
    cv::Mat colorImageToBoard(cv::Mat const& colorBoardImage);
    std::array<cv::Mat, 16> tileImages(cv::Mat boardImage);
    std::array<unsigned int, 16> boardState(cv::Mat boardImage, const std::vector<TileInfo>& canonicalTiles);
    MatchResult tileValue(cv::Mat tileImage, const std::vector<TileInfo>& canonicalTiles);
    
    const std::vector<TileInfo>& canonicalTiles();
    const cv::SIFT& sifter();
}

#endif /* defined(__ThreesAI__IMProc__) */
