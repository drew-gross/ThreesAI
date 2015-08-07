//
//  IMLog.h
//  ThreesAI
//
//  Created by Drew Gross on 8/6/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__IMLog__
#define __ThreesAI__IMLog__

#include <vector>
#include <numeric>

#include <opencv2/opencv.hpp>

namespace IMLog {
    cv::Mat concatH(std::vector<cv::Mat> v);
    cv::Mat concatV(std::vector<cv::Mat> v);
    void showContours(cv::Mat const image, std::vector<std::vector<cv::Point>> const contours);
}

#endif /* defined(__ThreesAI__IMLog__) */
