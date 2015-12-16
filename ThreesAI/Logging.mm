//
//  Logging.cpp
//  ThreesAI
//
//  Created by Drew Gross on 6/13/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "Logging.h"

#include <numeric> 
#include <ctime>
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

NSString *doshellscript(NSString *cmd, NSArray *args) {
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath: cmd];
    [task setArguments: args];
    NSPipe *pipe = [NSPipe pipe];
    [task setStandardOutput: pipe];
    [task launch];
    NSData *data = [[pipe fileHandleForReading] readDataToEndOfFile];
    NSString *string = [[NSString alloc] initWithData: data encoding: NSUTF8StringEncoding];
    return string;
}

void logGame(unsigned long score, time_t timeTaken) {
    PFObject *game = [PFObject objectWithClassName:@"Data"];
    game[@"Score"] = [NSNumber numberWithUnsignedLong:score];
    game[@"GitHash"] = doshellscript(@"/usr/local/bin/git", @[@"rev-parse", @"HEAD"]);
    [game save];
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