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

#include <opencv2/opencv.hpp>

class TileInfo {
public:
    TileInfo(cv::Mat image, int value);
    
    cv::Mat image;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    int value;
};

namespace IMProc {
    std::vector<cv::Point> findScreenContour(cv::Mat image);
    cv::Mat colorImageToBoard(cv::Mat colorBoardImage);
    const std::vector<TileInfo> loadCanonicalTiles();
    
    const std::vector<TileInfo> canonicalTiles = IMProc::loadCanonicalTiles();;
}

#endif /* defined(__ThreesAI__IMProc__) */