//
//  RandomAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "RandomAI.h"

#include <vector>

#include "TileStack.h"

using namespace std;

RandomAI::RandomAI(unique_ptr<ThreesBoardBase>&& board) : ThreesAIBase(move(board)) {}

Direction RandomAI::playTurn() {
    Direction move = this->board->randomValidMove();
    this->board->move(move);
    return move;
}