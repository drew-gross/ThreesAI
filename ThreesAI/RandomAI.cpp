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
    Direction d = static_cast<Direction>(std::uniform_int_distribution<>(0,3)(ThreesBoard::randomGenerator));
    this->board.processInputDirection(d);
}