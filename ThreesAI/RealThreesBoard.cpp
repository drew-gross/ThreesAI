;//
//  RealThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//2

#include "Debug.h"
#include "Logging.h"

#include <unistd.h>
#include <array>

#include <boost/multi_array.hpp>

#include "arduino-serial-lib.h"

#include "RealThreesBoard.h"
#include "SimulatedThreesBoard.h"

using namespace std;
using namespace cv;
using namespace boost;

TileInfo::TileInfo(cv::Mat image, int value) {
    this->image = image;
    this->value = value;
    IMProc::sifter().detect(image, this->keypoints);
    IMProc::sifter().compute(image, this->keypoints, this->descriptors);
}

const Point2f getpoint(const string& window) {
    Point2f p;
    setMouseCallback(window, [](int event, int x, int y, int flags, void* userdata){
        Point2f *p = (Point2f*)userdata;
        p->x = x;
        p->y = y;
    }, &p);
    waitKey();
    return p;
}

const std::array<Point2f, 4> getQuadrilateral(Mat m) {
    imshow("get rect", m);
    return std::array<cv::Point2f, 4>{{getpoint("get rect"),getpoint("get rect"),getpoint("get rect"),getpoint("get rect")}};
}

void RealThreesBoard::connectAndStart(string portName) {
    this->fd = serialport_init(portName.c_str(), 9600);
    sleep(2); //Necessary to initialize the output for some
    if (this->fd >= 0) {
        serialport_write(this->fd, "b");
        serialport_flush(this->fd);
    }
}

Mat RealThreesBoard::getAveragedImage(unsigned char numImages) {
    vector<Mat> images;
    for (unsigned char i = 0; i < numImages; i++) {
        Mat image;
        this->watcher >> image;
        images.push_back(image);
    }
    Mat averagedImage;
    if (numImages == 0) {
        return averagedImage;
    }
    averagedImage = Mat::zeros(images[0].rows, images[0].cols, images[0].type());
    for (unsigned char i = 0; i < numImages; i++) {
        averagedImage += images[i]/numImages;
    }
    return averagedImage;
}

RealThreesBoard::RealThreesBoard(string portName) : ThreesBoardBase({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}), watcher(0) {
    
    this->connectAndStart(portName);
    Mat boardImage(this->getAveragedImage(10));
    this->board = IMProc::boardState(IMProc::colorImageToBoard(boardImage), IMProc::canonicalTiles);
    MYSHOW(boardImage);
    waitKey();
}

RealThreesBoard::~RealThreesBoard() {
    if (this->fd >= 0) {
        serialport_close(this->fd);
    }
}

pair<unsigned int, ThreesBoardBase::BoardIndex> RealThreesBoard::move(Direction d) {
    if (this->fd >= 0) {
        switch (d) {
            case LEFT:
                serialport_write(this->fd, "l");
                break;
            case RIGHT:
                serialport_write(this->fd, "r");
                break;
            case UP:
                serialport_write(this->fd, "u");
                break;
            case DOWN:
                serialport_write(this->fd, "d");
                break;
        }
        
        serialport_flush(fd);
    }
    
    SimulatedThreesBoard expectedBoardAfterMove = this->simulatedCopy();
    expectedBoardAfterMove.moveWithoutAdd(d);
    vector<ThreesBoardBase::BoardIndex> possiblyEmptyTilesAfterMoveWithoutAdd = expectedBoardAfterMove.validIndicesForNewTile(d);
    
    Mat boardImage(this->getAveragedImage(5));
    this->isGameOverCacheIsValid = false;
    this->board = IMProc::boardState(IMProc::colorImageToBoard(boardImage), IMProc::canonicalTiles);
    if (!this->hasSameTilesAs(expectedBoardAfterMove, possiblyEmptyTilesAfterMoveWithoutAdd)) {
        MYSHOW(boardImage);
        debug();
    }
    
    //TODO: Get the actual location and value of the new tile
    return {0,{0,0}};
}

SimulatedThreesBoard RealThreesBoard::simulatedCopy() const {
    return SimulatedThreesBoard(this->board);
}

deque<unsigned int> RealThreesBoard::nextTileHint() const {
    //TODO: get this from the image;
    return {3};
}