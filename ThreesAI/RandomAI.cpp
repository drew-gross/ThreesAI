//
//  RandomAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "RandomAI.h"

#include <vector>

using namespace std;

RandomAI::RandomAI(BoardState board, unique_ptr<BoardOutput> output) : ThreesAIBase(board, move(output)) {}

Direction RandomAI::getDirection() const {
    default_random_engine generator;
    return this->currentState().randomValidMove(generator);
}