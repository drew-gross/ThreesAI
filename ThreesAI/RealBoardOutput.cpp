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

AddedTileInfo RealBoardOutput::computeChangeFrom(BoardState const& previousBoard) const {
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

std::shared_ptr<BoardState const> RealBoardOutput::currentState(HiddenBoardState otherInfo) const {
    return this->source->getGameState(otherInfo);
}

bool boardTransitionIsValid(BoardState const& oldBoard, Hint oldHint, Direction d, std::shared_ptr<BoardState const> newBoard) {
    EnabledIndices unknownIndexes = oldBoard.validIndicesForNewTile(d);
    //Check if any of the moved tiles don't read the same
    if (!newBoard->hasSameTilesAs(oldBoard, unknownIndexes)) {
        return false;
    }
    
    for (BoardIndex i : allIndices) {
        if (unknownIndexes.isEnabled(i) && newBoard->at(i) != Tile::EMPTY) {
            return oldHint.contains(newBoard->at(i));
        }
    }
    return false;
}

void RealBoardOutput::move(Direction d, BoardState const& originalBoard) {
    this->moveStepper(d);
    
    std::shared_ptr<BoardState const> expectedBoardAfterMove = make_shared<BoardState const>(BoardState::MoveWithoutAdd(d), originalBoard);
    
    std::shared_ptr<BoardState const> newState = this->source->getGameState(originalBoard.nextHiddenState(boost::none));
    
    if (newState->hasSameTilesAs(originalBoard, {})) {
        //Movement failed, retry.
        cout << "redo";
        return this->move(d, originalBoard);
    }
    //TODO: Detect if some other move was made accidentally, and just go with it.
    
    bool ok = boardTransitionIsValid(*expectedBoardAfterMove, originalBoard.getHint(), d, newState);
    
    if (!ok) {
        MYLOG(originalBoard);
        MYSHOWSMALL(originalBoard.sourceImage, 4);
        MYLOG(*newState);
        MYSHOWSMALL(newState->sourceImage, 4);
        MYLOG(d);
        MYLOG(*expectedBoardAfterMove);
        debug();
        IMProc::boardFromAnyImage(originalBoard.sourceImage, originalBoard.nextHiddenState(boost::none), *this->hintImages);
        IMProc::boardFromAnyImage(newState->sourceImage, originalBoard.nextHiddenState(boost::none), *this->hintImages);
        boardTransitionIsValid(*expectedBoardAfterMove, originalBoard.getHint(), d, newState);
    }
}

shared_ptr<BoardState const> RealBoardOutput::sneakyState() const {
    debug();
    return nullptr;
}

RealBoardOutput::RealBoardOutput(string port,
                                 shared_ptr<GameStateSource> source,
                                 BoardState const& initialState,
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