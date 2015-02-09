//
//  RandomAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "RandomAI.h"

RandomAI::RandomAI() : ThreesAIBase() {
    
}

void RandomAI::playTurn() {
    std::vector<Direction> moves = this->board.validMoves();
    std::shuffle(moves.begin(), moves.end(), ThreesBoard::randomGenerator);
    if (moves.empty()) {
        return;
    }
    this->board.tryMove(moves[0]);
}