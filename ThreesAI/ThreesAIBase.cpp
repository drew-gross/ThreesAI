//
//  ThreesAIBase.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesAIBase.h"

ThreesAIBase::ThreesAIBase(ThreesBoardBase *board) : board(board) {

}

void ThreesAIBase::playGame() {
    while (!this->board->isGameOver()) {
        this->playTurn();
    }
}