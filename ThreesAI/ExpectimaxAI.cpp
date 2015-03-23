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
    weak_ptr<ExpectimaxNodeBase> child;
    shared_ptr<ExpectimaxNodeBase> extantChild;
    while (n > 0) {
        MYLOG(this->unfilledChildren.size());
        child = this->unfilledChildren.front();
        while (child.expired()) {
            this->unfilledChildren.pop_front();
            child = this->unfilledChildren.front();
        }
        extantChild = child.lock();
        extantChild->fillInChildren(this->unfilledChildren);
        this->unfilledChildren.pop_front();
        n--;
    }
    unsigned int currentDepth = extantChild->depth;
    bool done = false;
    while (!done) {
        child = this->unfilledChildren.front();
        while (child.expired()) {
            this->unfilledChildren.pop_front();
            child = this->unfilledChildren.front();
        }
        extantChild = child.lock();
        if (extantChild->depth != currentDepth) {
            done = true;
        } else {
            extantChild->fillInChildren(this->unfilledChildren);
            this->unfilledChildren.pop_front();
        }
    }
}

Direction ExpectimaxAI::playTurn() {
    this->fillInChild(100);
    
    debug(this->board.numTurns == 24);
    pair<Direction, shared_ptr<const ExpectimaxNodeBase>> bestChild = this->currentBoard->maxChild();
    Direction bestDirection = bestChild.first;
    shared_ptr<const ExpectimaxNodeBase> bestResult = bestChild.second;
    
    pair<unsigned int, ThreesBoard::BoardIndex> addedTileInfo = this->board.move(bestDirection);
    unsigned int addedTileValue = addedTileInfo.first;
    ThreesBoard::BoardIndex addedTileLocation = addedTileInfo.second;
    
    shared_ptr<const ExpectimaxChanceNode> afterMoveBoard = dynamic_pointer_cast<const ExpectimaxChanceNode>(bestResult);
    
    shared_ptr<const ExpectimaxNodeBase> baseBoard = afterMoveBoard->child(ChanceNodeEdge(addedTileValue, addedTileLocation));

    afterMoveBoard->outputDot();
    shared_ptr<const ExpectimaxMoveNode> afterAddingTileBoard = dynamic_pointer_cast<const ExpectimaxMoveNode>(baseBoard);
    debug(afterAddingTileBoard == nullptr);
    
    this->currentBoard = const_pointer_cast<ExpectimaxMoveNode>(afterAddingTileBoard);
    return bestDirection;
}