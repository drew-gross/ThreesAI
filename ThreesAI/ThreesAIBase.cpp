//
//  ThreesAIBase.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesAIBase.h"

using namespace::std;

ThreesAIBase::ThreesAIBase(shared_ptr<ThreesBoardBase> new_board) : board(new_board) {}

void ThreesAIBase::playGame() {
    while (!this->board->isGameOver()) {
        this->playTurn();
    }
}