//
//  Logging.cpp
//  ThreesAI
//
//  Created by Drew Gross on 6/13/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "Logging.h"

#include <numeric> 
#include <sys/stat.h>

using namespace std;
using namespace cv;

void Log::imShowH(std::string name, vector<Mat> v) {
    int totalWidth = accumulate(v.begin(), v.end(), 0, [](int s, Mat m){
        return s + m.cols;
    });
    int maxHeight = std::max_element(v.begin(), v.end(), [](Mat first, Mat second){
        return first.rows < second.rows;
    })->rows;
    
    Mat combined(maxHeight, totalWidth, v[0].type());
    
    int widthSoFar = 0;
    for (auto it = v.begin(); it != v.end(); it++) {
        it->copyTo(combined(Rect(widthSoFar, 0, it->cols, it->rows)));
        widthSoFar += it->cols;
    }
    
    imshow(name, combined);
}
void Log::imShowV(std::string name, vector<Mat> v) {
    int totalHeight = accumulate(v.begin(), v.end(), 0, [](int s, Mat m){
        return s + m.rows;
    });
    int maxWidth = std::max_element(v.begin(), v.end(), [](Mat first, Mat second){
        return first.cols < second.cols;
    })->cols;
    
    Mat combined(totalHeight, maxWidth, v[0].type());
    
    int heightSoFar = 0;
    for (auto it = v.begin(); it != v.end(); it++) {
        it->copyTo(combined(Rect(0, heightSoFar, it->cols, it->rows)));
        heightSoFar += it->rows;
    }
    
    imshow(name, combined);
}

void Log::imShow(const string& winname, cv::InputArray image, double scale) {
    Mat smaller;
    resize(image, smaller, Size(), 1/scale, 1/scale);
    imshow(winname, smaller);
    
    string filename = "/tmp/threesAIdata/";
    mkdir(filename.c_str(), ACCESSPERMS);
    filename.append(winname);
    filename.append(".png");
    imwrite(filename, image);
}