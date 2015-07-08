;//
//  RealThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//2


#include "RealThreesBoard.h"

#include <unistd.h>
#include <array>

#include "arduino-serial-lib.h"

#include "SimulatedThreesBoard.h"

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace cv;

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
    this->image = this->getAveragedImage(8);
    this->board = IMProc::boardState(IMProc::colorImageToBoard(this->image), IMProc::canonicalTiles());
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
    
    //show old and new images
    Mat newImage(this->getAveragedImage(8));
    MYSHOW(newImage);
    MYSHOW(this->image);
    
    //show old new, and expected board
    SimulatedThreesBoard expectedBoardAfterMove = this->simulatedCopy();
    expectedBoardAfterMove.moveWithoutAdd(d);
    vector<ThreesBoardBase::BoardIndex> unknownIndexes = expectedBoardAfterMove.validIndicesForNewTile(d);
    Board newBoardState = IMProc::boardState(IMProc::colorImageToBoard(newImage), IMProc::canonicalTiles());
    MYLOG(this->board);
    MYLOG(newBoardState);
    MYLOG(expectedBoardAfterMove);
    
    if (!this->hasSameTilesAs(expectedBoardAfterMove, unknownIndexes)) {
        debug();
    }
    
    this->isGameOverCacheIsValid = false;
    this->image = newImage;
    this->board = newBoardState;
    
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