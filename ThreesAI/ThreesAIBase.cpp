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

#include "ChanceNodeEdge.h"

using namespace std;

ThreesAIBase::ThreesAIBase(shared_ptr<BoardState const> new_board, unique_ptr<BoardOutput> output) : boardState(new_board), boardOutput(move(output)) {}

shared_ptr<BoardState const> ThreesAIBase::currentState() const {
    return this->boardState;
}

void ThreesAIBase::playGame(bool printMove) {
    while (!this->boardState->isGameOver()) {
        if (printMove) {
            this->print = true;
            cout << "--- Currently ---" << endl;
            cout << *this->currentState() << endl;
        }
        this->playTurn();
    }
}

void ThreesAIBase::playTurn() {
    this->prepareDirection();
    Direction d = this->getDirection();
    if (this->print) {
        cout << "--- Moving: " << d << " ---" << endl;
    }
    this->boardOutput->move(d, *this->boardState);
    std::shared_ptr<BoardState const> newState = this->boardOutput->currentState(this->currentState()->nextHiddenState());
    this->boardState = newState;
    this->receiveState(d, *this->boardState);
}