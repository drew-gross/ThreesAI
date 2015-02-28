//
//  ExpectimaxAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxAI.h"

#include <time.h>

using namespace std;

ExpectimaxAI::ExpectimaxAI() : ThreesAIBase() {
    this->currentBoard = make_shared<ExpectimaxMoveNode>(this->board);
    this->unfilledChildren.push_back(this->currentBoard);
}

void ExpectimaxAI::fillInChild() {
    shared_ptr<ExpectimaxNodeBase> child = this->unfilledChildren.front();
    child->fillInChildren(this->unfilledChildren, UP);
    this->unfilledChildren.pop_front();
}

void ExpectimaxAI::playTurn() {
    clock_t analysisStartTime = clock();
    
    while (float(clock() - analysisStartTime)/CLOCKS_PER_SEC < .001) {
        this->fillInChild();
    }
    
    Direction d = this->currentBoard->maxChild().first;
    pair<unsigned int, ThreesBoard::BoardIndex> addedTileInfo = this->board.move(d);
    unsigned int addedTileValue = addedTileInfo.first;
    ThreesBoard::BoardIndex addedTileLocation = addedTileInfo.second;
    shared_ptr<ExpectimaxChanceNode> afterMoveBoard = dynamic_pointer_cast<ExpectimaxChanceNode>(this->currentBoard->child(d));
    shared_ptr<ExpectimaxMoveNode> afterAddingTileBoard = dynamic_pointer_cast<ExpectimaxMoveNode>(afterMoveBoard->child({addedTileValue, addedTileLocation, afterMoveBoard->board.tileStack.possibleUpcomingTiles(afterMoveBoard->board.maxTile()).front()}));
    this->currentBoard = afterAddingTileBoard;
}