//
//  Debug.cpp
//  ThreesAI
//
//  Created by Drew Gross on 3/1/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "Debug.h"

#include <opencv2/opencv.hpp>
#include <QuickTime/Movies.h>

void debug(bool cond) {
    if (cond) {
        cv::waitKey(1);
        cond=!cond;
    }
}