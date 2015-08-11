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

RealThreesBoard::RealThreesBoard(int fd, shared_ptr<VideoCapture> watcher, Board b, deque<unsigned int> initialHint) : ThreesBoardBase(b, initialHint), watcher(watcher), fd(fd) {}

shared_ptr<RealThreesBoard> RealThreesBoard::boardFromPortName(string port) {
    int fd = serialport_init(port.c_str(), 9600);
    sleep(2); //Necessary to initialize the output for some
    debug(fd < 0);
    
    shared_ptr<VideoCapture> watcher = make_shared<VideoCapture>(0);
    Mat initialImage = getAveragedImage(watcher, 8);
    auto state = boardState(screenImage(initialImage), canonicalTiles());

    return make_shared<RealThreesBoard>(fd, watcher, state.first, state.second);
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

bool boardTransitionIsValid(ThreesBoardBase const &oldBoard, MoveResult lastMove, ThreesBoardBase const &newBoard) {
    auto unknownIndexes = oldBoard.validIndicesForNewTile(lastMove.direction);
    //Check if any of the moved tiles don't read the same
    if (!newBoard.hasSameTilesAs(oldBoard, unknownIndexes)) {
        return false;
    }
    
    for (auto&& index : unknownIndexes) {
        if (newBoard.at(index) != 0) {
            for (auto&& hint : lastMove.hint) {
                if (newBoard.at(index) == hint) {
                    return true;
                }
            }
        }
    }
    return false;
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
    Mat newImage(getAveragedImage(this->watcher, 8));;
    
    //show old new, and expected board
    SimulatedThreesBoard expectedBoardAfterMove = this->simulatedCopy();
    expectedBoardAfterMove.moveWithoutAdd(d);
    vector<BoardIndex> unknownIndexes = expectedBoardAfterMove.validIndicesForNewTile(d);
    
    BoardInfo newBoardInfo = boardState(screenImage(newImage), canonicalTiles());
    SimulatedThreesBoard newBoardState(newBoardInfo.first, newBoardInfo.second);

    if (newBoardState.hasSameTilesAs(*this, {})) {
        //Movement failed, retry.
        return this->move(d);
    }
    
    bool ok = boardTransitionIsValid(*this, this->lastMove, newBoardState);
    
    if (!ok) {
        Log::imSave(newImage);
        Log::imSave(this->image);
        debug();
    }
    
    for (auto&& index : unknownIndexes) {
        unsigned int newTile = newBoardState.at(index);
        if (newTile != 0) {
            this->isGameOverCacheIsValid = false;
            this->image = newImage;
            this->board = newBoardState.board;
            this->lastMove = MoveResult(newTile, index, newBoardInfo.second, d);
            return this->lastMove;
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
    return MoveResult(0,{0,0},{0},d);
}

SimulatedThreesBoard RealThreesBoard::simulatedCopy() const {
    //TODO: copy the whole move result, not just the hint
    return SimulatedThreesBoard(this->board, this->lastMove.hint);
}