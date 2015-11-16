//
//  QuickTimeSource.cpp
//  ThreesAI
//
//  Created by Drew Gross on 8/18/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "QuickTimeSource.h"

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AppKit/AppKit.h>
#import <CoreServices/CoreServices.h>

#import <opencv2/opencv.hpp>

#import "Debug.h"

using namespace cv;

BOOL CGImageWriteToFile(CGImageRef image, NSString *path) {
    CFURLRef url = (__bridge CFURLRef)[NSURL fileURLWithPath:path];
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    if (!destination) {
        NSLog(@"Failed to create CGImageDestination for %@", path);
        return NO;
    }
    
    CGImageDestinationAddImage(destination, image, nil);
    
    if (!CGImageDestinationFinalize(destination)) {
        NSLog(@"Failed to write image to %@", path);
        CFRelease(destination);
        return NO;
    }
    
    CFRelease(destination);
    return YES;
}

cv::Mat getMostRecentFrame() {
    CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
    CFIndex windowCount = CFArrayGetCount(windows);
    for (CFIndex i = 0; i < windowCount; i++) {
        NSDictionary *window = (NSDictionary*)CFArrayGetValueAtIndex(windows, i);
        if ([window[(__bridge id)kCGWindowName] isEqualToString:@"Movie Recording"]) {
            CGRect bounds;
            CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)window[(__bridge id)kCGWindowBounds], &bounds);
            CGImageRef screenShot = CGWindowListCreateImage(CGRectNull, kCGWindowListOptionIncludingWindow, [window[@"kCGWindowNumber"] unsignedIntValue], kCGWindowImageBoundsIgnoreFraming);
            BOOL r = CGImageWriteToFile(screenShot, @"/tmp/blah.png");
            CGImageRelease(screenShot);
            debug(!r);
            CFRelease(windows);
            return cv::imread("/tmp/blah.png");
        }
    }
    CFRelease(windows);
    debug();
    return Mat();
}

QuickTimeSource::QuickTimeSource(){
}

std::shared_ptr<BoardState const> QuickTimeSource::getGameState(HiddenBoardState otherInfo) {
    return IMProc::boardFromAnyImage(getMostRecentFrame(), otherInfo);
}