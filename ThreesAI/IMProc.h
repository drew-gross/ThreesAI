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

namespace IMProc {
    std::vector<cv::Point> findScreenContour(cv::Mat const& image);
    cv::Mat colorImageToBoard(cv::Mat const& colorBoardImage);
    std::array<std::array<cv::Mat, 4>, 4> tileImages(cv::Mat boardImage);
    std::array<std::array<unsigned int, 4>, 4> boardState(cv::Mat boardImage, const std::vector<TileInfo>& canonicalTiles);
    int tileValue(cv::Mat tileImage, const std::vector<TileInfo>& canonicalTiles);
    const std::vector<TileInfo> loadCanonicalTiles();
    
    const std::vector<TileInfo> canonicalTiles = IMProc::loadCanonicalTiles();;
    
    const cv::SIFT& sifter();
}

#endif /* defined(__ThreesAI__IMProc__) */
