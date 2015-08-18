//
//  Capture.h
//  ThreesAI
//
//  Created by Drew Gross on 8/16/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef ThreesAI_Capture_h
#define ThreesAI_Capture_h

#ifdef __OBJC__

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AppKit/AppKit.h>

#import <opencv2/opencv.hpp>

#endif

cv::Mat getMostRecentFrame();

#endif
