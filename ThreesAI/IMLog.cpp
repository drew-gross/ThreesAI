//
//  IMLog.cpp
//  ThreesAI
//
//  Created by Drew Gross on 8/6/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "IMLog.h"

#include "Logging.h"

using namespace std;
using namespace cv;

Mat IMLog::concatH(vector<Mat> v) {
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
    
    return combined;
}

Mat IMLog::concatV(vector<Mat> v) {
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
    
    return combined;
}

void IMLog::showContours(Mat const image, vector<vector<Point>> const contours) {
    Mat contoursImage;
    image.copyTo(contoursImage);
    drawContours(contoursImage, contours, -1, Scalar(255), 5);
    MYSHOWSMALL(contoursImage,1);
}