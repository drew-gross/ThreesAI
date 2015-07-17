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

TileInfo::TileInfo(cv::Mat image, int value, const SIFT& sifter) {
    this->image = image;
    this->value = value;
    sifter.detect(image, this->keypoints);
    sifter.compute(image, this->keypoints, this->descriptors);
}
void RealThreesBoard::connectAndStart(string portName) {
    this->fd = serialport_init(portName.c_str(), 9600);
    sleep(2); //Necessary to initialize the output for some
    debug(this->fd < 0);
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
    for (auto&& image : images) {
        debug(image.rows != averagedImage.rows || image.cols != averagedImage.cols);
        averagedImage += image/numImages;
    }
    return averagedImage;
}

RealThreesBoard::RealThreesBoard(string portName) : ThreesBoardBase({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}), watcher(0) {
    this->connectAndStart(portName);
    this->image = this->getAveragedImage(8);
    this->board = IMProc::boardState(IMProc::colorImageToBoard(this->image), IMProc::canonicalTiles());
}

RealThreesBoard::~RealThreesBoard() {
    serialport_write(this->fd, "l");
    serialport_flush(fd);
    sleep(2);
    serialport_write(this->fd, "l");
    serialport_flush(fd);
    sleep(2);
    serialport_write(this->fd, "b");
    serialport_flush(fd);
    sleep(2);
    if (this->fd >= 0) {
        serialport_close(this->fd);
        sleep(2);
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
    
    //Wait 0.5s to allow image to stabilize
    usleep(500000);
    
    //show old and new images
    Mat newImage(this->getAveragedImage(8));;
    
    //show old new, and expected board
    SimulatedThreesBoard expectedBoardAfterMove = this->simulatedCopy();
    expectedBoardAfterMove.moveWithoutAdd(d);
    vector<ThreesBoardBase::BoardIndex> unknownIndexes = expectedBoardAfterMove.validIndicesForNewTile(d);
    SimulatedThreesBoard newBoardState = SimulatedThreesBoard(IMProc::boardState(IMProc::colorImageToBoard(newImage), IMProc::canonicalTiles()));

    if (newBoardState.hasSameTilesAs(*this, {})) {
        //Movement failed, retry.
        return this->move(d);
    }
    
    if (!newBoardState.hasSameTilesAs(expectedBoardAfterMove, unknownIndexes)) {
        MYSHOW(newImage);
        MYSHOW(this->image)
        MYLOG(this->board);
        MYLOG(newBoardState);
        MYLOG(expectedBoardAfterMove);
        debug();
        newBoardState.hasSameTilesAs(expectedBoardAfterMove, unknownIndexes);
    }
    
    this->isGameOverCacheIsValid = false;
    this->image = newImage;
    this->board = newBoardState.board;
    
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