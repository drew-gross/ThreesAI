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

SimulatedBoardOutput::SimulatedBoardOutput(BoardState::Board otherBoard, default_random_engine hintGen, unsigned int ones, unsigned int twos, unsigned int threes) : BoardOutput(), state(otherBoard, hintGen, 0, Mat(), ones, twos, threes) {};

unique_ptr<SimulatedBoardOutput> SimulatedBoardOutput::randomBoard() {
    static default_random_engine shuffler;
    std::array<unsigned int, 16> initialTiles = {3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0};
    shuffle(initialTiles.begin(), initialTiles.end(), shuffler);
    return unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(initialTiles, shuffler, 4, 4, 4));
}

BoardState SimulatedBoardOutput::currentState() const {
    return this->state;
}

void SimulatedBoardOutput::move(Direction d, BoardState const& originalBoard) {
    this->state = this->currentState().moveWithoutAdd(d).addTile(d);
}