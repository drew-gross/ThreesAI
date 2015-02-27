//
//  RandomAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "RandomAI.h"

using namespace std;

RandomAI::RandomAI() : ThreesAIBase() {
    
}

void RandomAI::playTurn() {
    vector<Direction> moves = this->board.validMoves();
    shuffle(moves.begin(), moves.end(), TileStack::randomGenerator);
    if (moves.empty()) {
        return;
    }
    this->board.move(moves[0]);
}