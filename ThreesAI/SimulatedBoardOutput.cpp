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

using namespace std;
using namespace cv;

SimulatedBoardOutput::SimulatedBoardOutput(BoardState::Board otherBoard, default_random_engine hintGen, unsigned int ones, unsigned int twos, unsigned int threes) : BoardOutput(), state(make_shared<BoardState>(otherBoard, hintGen, 0, Mat(), ones, twos, threes)) {};

SimulatedBoardOutput::SimulatedBoardOutput(shared_ptr<BoardState const> b) : state(b) {}

unique_ptr<SimulatedBoardOutput> SimulatedBoardOutput::randomBoard(default_random_engine shuffler) {
    std::array<Tile, 16> initialTiles = {Tile::TILE_3,Tile::TILE_3,Tile::TILE_3,Tile::TILE_2,Tile::TILE_2,Tile::TILE_2,Tile::TILE_1,Tile::TILE_1,Tile::TILE_1,Tile::EMPTY,Tile::EMPTY,Tile::EMPTY,Tile::EMPTY,Tile::EMPTY,Tile::EMPTY,Tile::EMPTY};
    shuffle(initialTiles.begin(), initialTiles.end(), shuffler);
    return unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(initialTiles, shuffler, 4, 4, 4));
}

shared_ptr<BoardState const> SimulatedBoardOutput::currentState() const {
    return this->state;
}

void SimulatedBoardOutput::move(Direction d, BoardState const& originalBoard) {
    this->state = make_shared<BoardState>(BoardState::Move(d), *this->currentState());
}