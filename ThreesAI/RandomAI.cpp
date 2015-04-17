//
//  RandomAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <vector>

#include "RandomAI.h"
#include "TileStack.h"

using namespace std;

RandomAI::RandomAI(unique_ptr<ThreesBoardBase>&& board) : ThreesAIBase(move(board)) {
    
}

Direction RandomAI::playTurn() {
    vector<Direction> moves = this->board->validMoves();
    shuffle(moves.begin(), moves.end(), TileStack::randomGenerator);
    this->board->move(moves[0]);
    return moves[0];
}