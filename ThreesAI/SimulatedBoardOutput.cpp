//
//  SimulatedBoardOutput.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "SimulatedBoardOutput.h"

#include "Debug.h"
#include "Logging.h"

#include <chrono>

using namespace std;
using namespace chrono;
using namespace cv;

SimulatedBoardOutput::SimulatedBoardOutput(Board otherBoard,
                                           default_random_engine hintGen,
                                           unsigned int ones,
                                           unsigned int twos,
                                           unsigned int threes) :
BoardOutput(),
state(make_shared<AboutToMoveBoard>(otherBoard, HiddenBoardState(0, ones, twos, threes), hintGen, Mat())) {
};

SimulatedBoardOutput::SimulatedBoardOutput(BoardStateCPtr b) : BoardOutput(), state(b) {}

std::shared_ptr<AboutToMoveBoard const> SimulatedBoardOutput::sneakyState() const {
    return this->state;
}

AddedTileInfo SimulatedBoardOutput::computeChangeFrom(AboutToAddTileBoard const& boardWithoutTile) const {
    return AddedTileInfo(boardWithoutTile, *this->state);
}

unique_ptr<SimulatedBoardOutput> SimulatedBoardOutput::randomBoard(default_random_engine& shuffler) {
    std::array<Tile, 16> initialTiles = {
        T::_3,
        T::_3,
        T::_3,
        T::_2,
        T::_2,
        T::_2,
        T::_1,
        T::_1,
        T::_1,
        T::EMPTY,
        T::EMPTY,
        T::EMPTY,
        T::EMPTY,
        T::EMPTY,
        T::EMPTY,
        T::EMPTY
    };
    shuffle(initialTiles.begin(), initialTiles.end(), shuffler);
    return unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(Board(initialTiles), shuffler, 1, 1, 1));
}

shared_ptr<AboutToMoveBoard const> SimulatedBoardOutput::currentState(HiddenBoardState otherInfo) const {
    //debug(!(otherInfo == this->state->hiddenState)); TODO(drewgross): add tests to make sure initializers ser hiddenstate correctly
    return this->state;
}

void SimulatedBoardOutput::move(Direction d, AboutToMoveBoard const& originalBoard) {
    this->state = make_shared<AboutToMoveBoard>(AboutToMoveBoard::MoveWithAdd(d), *this->currentState(originalBoard.hiddenState));
    this->doWorkFor(0);
}