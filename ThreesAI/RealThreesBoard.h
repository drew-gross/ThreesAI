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

class TileInfo {
public:
    TileInfo(cv::Mat image, int value);
    
    cv::Mat image;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    int value;
};


class RealThreesBoard : public ThreesBoardBase {
public:
    RealThreesBoard(std::string serialPath);
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    std::pair<unsigned int, BoardIndex> move(Direction d);
    std::deque<unsigned int> nextTileHint() const;
    SimulatedThreesBoard simulatedCopy() const;
    
    ~RealThreesBoard();
    
private:
    cv::VideoCapture watcher;
    
    cv::Mat captureBoardImage();

    const std::vector<TileInfo> canonicalTiles;
    const std::vector<TileInfo> loadCanonicalTiles();
    
    int fd;
    
    void connectAndStart(std::string portName);
};

#endif /* defined(__ThreesAI__RealThreesBoard__) */
