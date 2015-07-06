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
    void imageVector(std::vector<cv::Mat> v);
    void imShow(const std::string& winname, cv::InputArray image, double scale=1);
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

template <typename T>
std::ostream& operator<<(std::ostream &os, const std::array<T, 16>& tiles){
    os << "---------------------" << std::endl;
    os << "|" << std::setw(4) << tiles[0] << "|" << std::setw(4) << tiles[1] << "|" << std::setw(4) << tiles[2] << "|" << std::setw(4) << tiles[3] << "|" << std::endl;
    os << "|" << std::setw(4) << tiles[4] << "|" << std::setw(4) << tiles[5] << "|" << std::setw(4) << tiles[6] << "|" << std::setw(4) << tiles[7] << "|" << std::endl;
    os << "|" << std::setw(4) << tiles[8] << "|" << std::setw(4) << tiles[9] << "|" << std::setw(4) << tiles[10] << "|" << std::setw(4) << tiles[11] << "|" << std::endl;
    os << "|" << std::setw(4) << tiles[12] << "|" << std::setw(4) << tiles[13] << "|" << std::setw(4) << tiles[14] << "|" << std::setw(4) << tiles[15] << "|" << std::endl;
    os << "---------------------";
    return os;
}

#endif
