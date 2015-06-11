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

namespace IMProc {
    std::vector<cv::Point> findScreenContour(cv::Mat image);
}

#endif /* defined(__ThreesAI__IMProc__) */
