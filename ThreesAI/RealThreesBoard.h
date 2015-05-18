//
//  RealThreesBoard.h
//  ThreesAI
//
//  Created by Drew Gross on 4/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__RealThreesBoard__
#define __ThreesAI__RealThreesBoard__

#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "ThreesBoardBase.h"


class RealThreesBoard : public ThreesBoardBase {
public:
    RealThreesBoard(std::string serialPath);
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    std::pair<unsigned int, BoardIndex> move(Direction d);
    std::deque<unsigned int> nextTileHint() const;
    SimulatedThreesBoard simulatedCopy() const;
    
    ~RealThreesBoard();
    
    int fd;
private:
    cv::VideoCapture watcher;
    cv::Mat boardImage;

    const std::vector<cv::Mat> sampleImages;
    std::vector<std::vector<cv::KeyPoint>> sampleKeyPoints; //TOOD: make const
    std::vector<cv::Mat> sampleDescriptors; //TODO: make const
    static const std::vector<cv::Mat> loadSampleImages();
};

#endif /* defined(__ThreesAI__RealThreesBoard__) */
