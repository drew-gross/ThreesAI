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

SimulatedBoardOutput::SimulatedBoardOutput(BoardState::Board otherBoard,
                                           default_random_engine hintGen,
                                           unsigned int ones,
                                           unsigned int twos,
                                           unsigned int threes) :
BoardOutput(),
state(make_shared<BoardState>(otherBoard, HiddenBoardState(0, ones, twos, threes), hintGen, Mat())) {};

SimulatedBoardOutput::SimulatedBoardOutput(BoardStateCPtr b) : BoardOutput(), state(b) {}

std::shared_ptr<BoardState const> SimulatedBoardOutput::sneakyState() const {
    return this->state;
}

AddedTileInfo SimulatedBoardOutput::computeChangeFrom(BoardState const& boardWithoutTile) const {
    return AddedTileInfo(boardWithoutTile, *this->state);
}

unique_ptr<SimulatedBoardOutput> SimulatedBoardOutput::randomBoard(default_random_engine shuffler) {
    std::array<Tile, 16> initialTiles = {
        Tile::TILE_3,
        Tile::TILE_3,
        Tile::TILE_3,
        Tile::TILE_2,
        Tile::TILE_2,
        Tile::TILE_2,
        Tile::TILE_1,
        Tile::TILE_1,
        Tile::TILE_1,
        Tile::EMPTY,
        Tile::EMPTY,
        Tile::EMPTY,
        Tile::EMPTY,
        Tile::EMPTY,
        Tile::EMPTY,
        Tile::EMPTY
    };
    shuffle(initialTiles.begin(), initialTiles.end(), shuffler);
    return unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(initialTiles, shuffler, 1, 1, 1));
}

shared_ptr<BoardState const> SimulatedBoardOutput::currentState(HiddenBoardState otherInfo) const {
    debug(!(otherInfo == this->state->hiddenState));
    return this->state;
}

void SimulatedBoardOutput::move(Direction d, BoardState const& originalBoard) {
    this->state = make_shared<BoardState>(BoardState::MoveWithAdd(d), *this->currentState(originalBoard.hiddenState));
    this->doWorkFor(1);
}