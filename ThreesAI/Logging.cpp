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

string gen_random(const int len) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string s;
    for (int i = 0; i < len; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
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

void Log::imSave(Mat image) {
    string filename = Log::project_path;
    filename.append("imagesForReview/");
    filename.append(gen_random(8));
    filename.append(".png");
    imwrite(filename, image);
}