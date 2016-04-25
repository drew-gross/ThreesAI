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
#include <exception>

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace chrono;

AddedTileInfo RealBoardOutput::computeChangeFrom(AboutToAddTileBoard const& previousBoard) const {
    return AddedTileInfo(previousBoard, *this->source->getGameState(HiddenBoardState(0,1,1,1)));
}

void RealBoardOutput::pressWithServo() {
    if (this->fd >= 0) {
        serialport_write(this->fd, "b");
        serialport_flush(fd);
        this->doWorkFor(2000);
    }
}

void RealBoardOutput::moveStepper(Direction d) {
    if (this->fd >= 0) {
        switch (d) {
            case Direction::LEFT:
                serialport_write(this->fd, "l");
                break;
            case Direction::RIGHT:
                serialport_write(this->fd, "r");
                break;
            case Direction::UP:
                serialport_write(this->fd, "u");
                break;
            case Direction::DOWN:
                serialport_write(this->fd, "d");
                break;
        }
        serialport_flush(fd);
        this->doWorkFor(2000);
    }
}

std::shared_ptr<AboutToMoveBoard const> RealBoardOutput::currentState(HiddenBoardState otherInfo) const {
    return this->source->getGameState(otherInfo);
}

bool boardTransitionIsValid(AboutToAddTileBoard const& oldBoard, Hint oldHint, std::shared_ptr<AboutToMoveBoard const> newBoard) {
    debug();
    //TODO: enable this when I put it on the real iPhone
    EnabledIndices unknownIndexes = oldBoard.validIndicesForNewTile;
    //Check if any of the moved tiles don't read the same
    if (!newBoard->hasSameTilesAs(oldBoard)) {
        return false;
    }
    
    for (BoardIndex i : allIndices) {
        if (unknownIndexes.isEnabled(i) && newBoard->at(i) != T::EMPTY) {
            return oldHint.contains(newBoard->at(i));
        }
    }
    return false;
}

void RealBoardOutput::move(Direction d, AboutToMoveBoard const& originalBoard) {
    this->moveStepper(d);
    
    std::shared_ptr<AboutToAddTileBoard const> expectedBoardAfterMove = make_shared<AboutToAddTileBoard const>(originalBoard.moveWithoutAdd(d, false));
    
    std::shared_ptr<AboutToMoveBoard const> newState = this->source->getGameState(expectedBoardAfterMove->hiddenState);
    
    if (newState->hasSameTilesAs(originalBoard)) {
        //Movement failed, retry.
        cout << "redo";
        return this->move(d, originalBoard);
    }
    //TODO: Detect if some other move was made accidentally, and just go with it.
    
    bool ok = boardTransitionIsValid(*expectedBoardAfterMove, originalBoard.getHint(), newState);
    
    if (!ok) {
        MYLOG(originalBoard);
        MYSHOWSMALL(originalBoard.sourceImage, 4);
        MYLOG(*newState);
        MYSHOWSMALL(newState->sourceImage, 4);
        MYLOG(d);
        //TODO: Log the real thing
        MYLOG(&expectedBoardAfterMove->board);
        debug();
        IMProc::boardFromAnyImage(originalBoard.sourceImage, expectedBoardAfterMove->hiddenState, *this->hintImages);
        IMProc::boardFromAnyImage(newState->sourceImage, expectedBoardAfterMove->hiddenState, *this->hintImages);
        boardTransitionIsValid(*expectedBoardAfterMove, originalBoard.getHint(), newState);
    }
}

shared_ptr<AboutToMoveBoard const> RealBoardOutput::sneakyState() const {
    debug();
    return nullptr;
}

RealBoardOutput::RealBoardOutput(string port,
                                 shared_ptr<GameStateSource> source,
                                 AboutToMoveBoard const& initialState,
                                 shared_ptr<HintImages const> hintImages) :
BoardOutput(),
source(std::move(source)),
hintImages(hintImages) {
    this->fd = serialport_init(port.c_str(), 9600);
    if (this->fd < 0) {
        throw std::exception();
    }
    sleep(2); //Necessary to initialize the output
}