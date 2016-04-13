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

ThreesAIBase::ThreesAIBase(shared_ptr<AboutToMoveBoard const> new_board, unique_ptr<BoardOutput> output) : boardState(new_board), boardOutput(move(output)) {}

shared_ptr<AboutToMoveBoard const> ThreesAIBase::currentState() const {
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
        if (printMove) {
            start = time(nullptr);
        }
        this->playTurn(printMove);
        if (printMove) {
            end = time(nullptr);
        }
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
    AboutToAddTileBoard movedBoard = this->boardState->moveWithoutAdd(d, false);
    AddedTileInfo mostRecentlyAddedTile = this->boardOutput->computeChangeFrom(movedBoard);
    this->boardState = make_shared<AboutToMoveBoard const>(movedBoard.addSpecificTile(mostRecentlyAddedTile));
    this->receiveState(d, *this->boardState);
}