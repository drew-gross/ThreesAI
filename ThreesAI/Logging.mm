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

#import <Parse/Parse.h>

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

void initParse(string appID, string clientKey) {
    [Parse setApplicationId:[NSString stringWithCString:appID.c_str() encoding:NSUTF8StringEncoding] clientKey:[NSString stringWithCString:clientKey.c_str() encoding:NSUTF8StringEncoding]];
}

void Log::imShow(const string& winname, cv::InputArray image, double scale) {
    Mat smaller;
    resize(image, smaller, cv::Size(), 1/scale, 1/scale, INTER_NEAREST);
    imshow(winname, smaller);
    
    string filename = "/tmp/threesAIdata/";
    mkdir(filename.c_str(), ACCESSPERMS);
    filename.append(winname);
    filename.append(".png");
    imwrite(filename, image);
    waitKey(1);
}

void Log::imSave(Mat image) {
    string filename = Log::project_path;
    filename.append("imagesForReview/");
    filename.append(gen_random(8));
    filename.append(".png");
    imwrite(filename, image);
}