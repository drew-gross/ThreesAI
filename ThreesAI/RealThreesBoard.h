//
//  RealThreesBoard.h
//  ThreesAI
//
//  Created by Drew Gross on 4/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__RealThreesBoard__
#define __ThreesAI__RealThreesBoard__

#include <opencv2/opencv.hpp>

#include "ThreesBoardBase.h"
#include "IMProc.h"

class RealThreesBoard : public ThreesBoardBase {
public:
    static std::pair<RealThreesBoard, std::deque<unsigned int>> boardFromPortName(std::string serialPath);
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    MoveResult move(Direction d);
    std::deque<unsigned int> nextTileHint() const;
    SimulatedThreesBoard simulatedCopy() const;
    
    ~RealThreesBoard();
    
private:
    RealThreesBoard(std::string serialPath);
    cv::VideoCapture watcher;
    
    cv::Mat getAveragedImage(unsigned char numImages);
    cv::Mat image;
    
    int fd;
    
    void connectAndStart(std::string portName);
};

#endif /* defined(__ThreesAI__RealThreesBoard__) */
