//
//  ExpectimaxAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxAI.h"

#include <time.h>

#include "Debug.h"
#include "Logging.h"

using namespace std;

ExpectimaxAI::ExpectimaxAI() : ThreesAIBase(), currentBoard(make_shared<ExpectimaxMoveNode>(this->board, 0)) {
    this->unfilledChildren.push_back(this->currentBoard);
}

void ExpectimaxAI::fillInChild(unsigned int n) {
    if (n == 0) {
        return;
    }
    weak_ptr<ExpectimaxNodeBase> possibleChild;
    while (possibleChild.expired()) {
        if (this->unfilledChildren.empty()) {
            return;
        }
        possibleChild = this->unfilledChildren.front();
        this->unfilledChildren.pop_front();
    }
    shared_ptr<ExpectimaxNodeBase> child = possibleChild.lock();
    child->fillInChildren(this->unfilledChildren);
    this->fillInChild(n - 1);
    //TODO: maybe I need to make sure the depth is the same everywhere?
    return;
}

Direction ExpectimaxAI::playTurn() {
    this->fillInChild(50);
    this->currentBoard->pruneUnreachableChildren(this->board.nextTileHint());
    if (this->board.numTurns == 76) {
        this->currentBoard->outputDot();
    }
    
    pair<Direction, shared_ptr<const ExpectimaxNodeBase>> bestChild = this->currentBoard->maxChild();
    Direction bestDirection = bestChild.first;
    shared_ptr<const ExpectimaxChanceNode> afterMoveBoard = dynamic_pointer_cast<const ExpectimaxChanceNode>(bestChild.second);
    
    pair<unsigned int, ThreesBoard::BoardIndex> addedTileInfo = this->board.move(bestDirection);
    unsigned int addedTileValue = addedTileInfo.first;
    ThreesBoard::BoardIndex addedTileLocation = addedTileInfo.second;
    
    shared_ptr<const ExpectimaxNodeBase> baseBoard = afterMoveBoard->child(ChanceNodeEdge(addedTileValue, addedTileLocation));

    shared_ptr<const ExpectimaxMoveNode> afterAddingTileBoard = dynamic_pointer_cast<const ExpectimaxMoveNode>(baseBoard);
    
    this->currentBoard = const_pointer_cast<ExpectimaxMoveNode>(afterAddingTileBoard);
    return bestDirection;
}