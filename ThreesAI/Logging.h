//
//  Logging.h
//  ThreesAI
//
//  Created by Drew Gross on 2/2/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef ThreesAI_Logging_h
#define ThreesAI_Logging_h

#define MYLOG(x) (std::cout << "Value of " << #x << " is: " << x << std::endl);
#define MYSHOW(x) Log::imShow(#x, x);
#define MYSHOWSMALL(i, s) Log::imShow(#i, i, s);

#include <iomanip>

#include <vector>
#include <array>

#include <opencv2/opencv.hpp>

namespace Log {
    void imShow(const std::string& winname, cv::InputArray image, double scale=1);
    void imSave(cv::Mat image);
    
    const std::string project_path = "/Users/drewgross/Projects/ThreesAI/";
}

template <typename T>
std::ostream& operator<<(std::ostream &os, const std::deque<T> d){
    os << "[";
    for (auto&& t : d) {
        os << " " << t;
    }
    os << "]";
    return os;
}

#endif
