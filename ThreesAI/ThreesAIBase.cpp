//
//  ThreesAIBase.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesAIBase.h"

#include "Debug.h"
#include "Logging.h"

using namespace std;

ThreesAIBase::ThreesAIBase(BoardState new_board, unique_ptr<BoardOutput> output) : boardState(new_board), boardOutput(move(output)) {}

BoardState ThreesAIBase::currentState() const {
    return this->boardState;
}

void ThreesAIBase::playGame(bool printMove) {
    while (!this->boardState.isGameOver()) {
        this->playTurn();
        if (printMove) {
            cout << this->currentState() << endl;
        }
    }
}

void ThreesAIBase::playTurn() {
    this->prepareDirection();
    Direction d = this->getDirection();
    this->boardOutput->move(d, this->boardState);
    BoardState newState = this->boardOutput->currentState();
    this->receiveState(d, newState);
    this->boardState = newState;
}