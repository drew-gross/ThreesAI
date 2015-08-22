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

RealThreesBoard::RealThreesBoard(string port, unique_ptr<GameStateSource> source, BoardInfo initialState) : ThreesBoardBase(initialState.tiles, initialState.nextTileHint), source(std::move(source)), oldState(initialState) {
    this->fd = serialport_init(port.c_str(), 9600);
    sleep(2); //Necessary to initialize the output
    debug(this->fd < 0);
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

bool boardTransitionIsValid(ThreesBoardBase const &oldBoard, deque<unsigned int> const& oldHint, Direction d, ThreesBoardBase const &newBoard) {
    auto unknownIndexes = oldBoard.validIndicesForNewTile(d);
    //Check if any of the moved tiles don't read the same
    if (!newBoard.hasSameTilesAs(oldBoard, unknownIndexes)) {
        return false;
    }
    
    for (auto&& index : unknownIndexes) {
        if (newBoard.at(index) != 0) {
            for (auto&& hint : oldHint) {
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
    
    SimulatedThreesBoard expectedBoardAfterMove = this->simulatedCopy();
    expectedBoardAfterMove.moveWithoutAdd(d);
    vector<BoardIndex> unknownIndexes = expectedBoardAfterMove.validIndicesForNewTile(d);
    
    BoardInfo newBoardInfo = this->source->getGameState();
    
    SimulatedThreesBoard newBoardState(newBoardInfo.tiles, newBoardInfo.nextTileHint);
    
    if (newBoardState.hasSameTilesAs(*this, {})) {
        //Movement failed, retry.
        return this->move(d);
    }
    
    bool ok = boardTransitionIsValid(expectedBoardAfterMove, this->lastMove.hint, d, newBoardState);
    
    if (!ok) {
        MYLOG(this->oldState);
        MYSHOWSMALL(this->oldState.image, 4);
        MYLOG(newBoardInfo);
        MYSHOWSMALL(newBoardInfo.image, 4);
        debug();
        boardTransitionIsValid(expectedBoardAfterMove, this->lastMove.hint, d, newBoardState);
    }
    
    for (auto&& index : unknownIndexes) {
        unsigned int newTile = newBoardState.at(index);
        if (newTile != 0) {
            this->isGameOverCacheIsValid = false;
            this->board = newBoardState.board;
            this->oldState = newBoardInfo;
            this->lastMove = MoveResult(newTile, index, newBoardInfo.nextTileHint, d);
            return this->lastMove;
        }
    }
    debug();
    return MoveResult(0,{0,0},{0},d);
}

SimulatedThreesBoard RealThreesBoard::simulatedCopy() const {
    //TODO: copy the whole move result, not just the hint
    return SimulatedThreesBoard(this->board, this->lastMove.hint);
}