//
//  RealBoardOutput.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "RealBoardOutput.h"
#include "arduino-serial-lib.h"
#include <unistd.h>

#include "Debug.h"
#include "Logging.h"

using namespace std;

RealBoardOutput::~RealBoardOutput() {
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

bool boardTransitionIsValid(BoardState const &oldBoard, Hint const& oldHint, Direction d, BoardState const &newBoard) {
    auto unknownIndexes = oldBoard.validIndicesForNewTile(d);
    //Check if any of the moved tiles don't read the same
    if (!newBoard.hasSameTilesAs(oldBoard, unknownIndexes)) {
        return false;
    }
    
    for (auto&& index : unknownIndexes) {
        if (newBoard.at(index) != 0) {
            return (oldHint.contains(newBoard.at(index)));
        }
    }
    return false;
}

void RealBoardOutput::move(Direction d, BoardState const& originalBoard) {
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
    
    BoardState expectedBoardAfterMove = originalBoard.moveWithoutAdd(d);
    vector<BoardState::BoardIndex> unknownIndexes = expectedBoardAfterMove.validIndicesForNewTile(d);
    
    BoardState newState = this->source->getGameState();
    
    if (newState.hasSameTilesAs(originalBoard, {})) {
        //Movement failed, retry.
        return this->move(d, originalBoard);
    }
    
    bool ok = boardTransitionIsValid(expectedBoardAfterMove, originalBoard.getHint(), d, newState);
    
    if (!ok) {
        MYLOG(originalBoard);
        MYSHOWSMALL(originalBoard.sourceImage, 4);
        MYLOG(newState);
        MYSHOWSMALL(newState.sourceImage, 4);
        debug();
        IMProc::boardFromAnyImage(originalBoard.sourceImage);
        IMProc::boardFromAnyImage(newState.sourceImage);
        boardTransitionIsValid(expectedBoardAfterMove, originalBoard.getHint(), d, newState);
    }
}

RealBoardOutput::RealBoardOutput(string port, shared_ptr<GameStateSource> source, BoardState initialState) : BoardOutput(), source(std::move(source)) {
    this->fd = serialport_init(port.c_str(), 9600);
    sleep(2); //Necessary to initialize the output
    debug(this->fd < 0);
}