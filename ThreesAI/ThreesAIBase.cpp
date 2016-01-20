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

#include "AddedTileInfo.h"

using namespace std;

ThreesAIBase::ThreesAIBase(shared_ptr<BoardState const> new_board, unique_ptr<BoardOutput> output) : boardState(new_board), boardOutput(move(output)) {}

shared_ptr<BoardState const> ThreesAIBase::currentState() const {
    return this->boardState;
}

void ThreesAIBase::playGame(bool printMove, bool inspectMove) {
    time_t start = 0;
    time_t end = 0;
    while (!this->boardState->isGameOver()) {
        if (printMove) {
            cout << "--- Currently ---" << endl;
            cout << *this->currentState() << endl;
            cout << "Time evaluating: " << end - start << "s" << endl;
        }
        if (inspectMove) {
            getchar();
        }
        start = time(nullptr);
        this->playTurn();
        end = time(nullptr);
    }
    if (printMove) {
        cout << "--- Currently ---" << endl;
        cout << *this->currentState() << endl;
    }
}

void ThreesAIBase::playTurn(bool printMove) {
    this->prepareDirection();
    Direction d = this->getDirection();
    if (printMove) {
        cout << "--- Moving: " << d << " ---" << endl;
    }
    this->boardOutput->move(d, *this->boardState);
    Tile mostRecentlyAddedTile = this->boardOutput->computeChangeFrom(BoardState(BoardState::MoveWithoutAdd(d), *this->boardState)).newTileValue;
    this->boardState = this->boardOutput->currentState(this->currentState()->nextHiddenState(mostRecentlyAddedTile));
    this->receiveState(d, *this->boardState);
}