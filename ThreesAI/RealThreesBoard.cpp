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
using namespace IMProc;

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
    auto state = boardState(screenImage(this->image), canonicalTiles());
    this->board = state.first;
//TODO    this->cachedTileHint = state.second;
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

MoveResult RealThreesBoard::move(Direction d) {
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
    vector<BoardIndex> unknownIndexes = expectedBoardAfterMove.validIndicesForNewTile(d);
    
    BoardInfo newBoardInfo = boardState(screenImage(newImage), canonicalTiles());
    SimulatedThreesBoard newBoardState(newBoardInfo.first);

    if (newBoardState.hasSameTilesAs(*this, {})) {
        //Movement failed, retry.
        return this->move(d);
    }
    
    //Check if any of the moved tiles don't read the same
    if (!newBoardState.hasSameTilesAs(expectedBoardAfterMove, unknownIndexes)) {
        Log::imSave(newImage);
        Log::imSave(this->image);
    }
    
    //TODO: Check if the new tile matches the expected one from the hint
    
    for (auto&& index : unknownIndexes) {
        if (newBoardState.at(index) != 0) {
            this->isGameOverCacheIsValid = false;
            this->image = newImage;
            this->board = newBoardState.board;
            debug(newBoardState.at(index) == 0);
            return {newBoardState.at(index), index, newBoardInfo.second};
        }
    }
    Log::imSave(newImage);
    Log::imSave(this->image);
    /*
    MYSHOW(newImage);
    MYSHOW(this->image)
    MYLOG(newBoardState);
    MYLOG(*this);
    MYLOG(expectedBoardAfterMove);*/
    debug();
    return MoveResult(0,{0,0},{0});
}

SimulatedThreesBoard RealThreesBoard::simulatedCopy() const {
    return SimulatedThreesBoard(this->board);
}