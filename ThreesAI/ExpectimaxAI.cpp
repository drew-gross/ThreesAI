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

ExpectimaxAI::ExpectimaxAI() : ThreesAIBase() {
    this->currentBoard = make_shared<ExpectimaxMoveNode>(this->board);
    this->unfilledChildren.push_back(this->currentBoard);
}

void ExpectimaxAI::fillInChild(unsigned int n) {
    while (n > 0) {
        weak_ptr<ExpectimaxNodeBase> child = this->unfilledChildren.front();
        while (child.expired()) {
            this->unfilledChildren.pop_front();
            child = this->unfilledChildren.front();
        }
        shared_ptr<ExpectimaxNodeBase> extantChild = child.lock();
        extantChild->fillInChildren(this->unfilledChildren);
        this->unfilledChildren.pop_front();
        n--;
    }
}

Direction ExpectimaxAI::playTurn() {
    this->fillInChild(1000);
    
    if (!this->currentBoard->childrenAreFilledIn()) {
        this->currentBoard->fillInChildren(this->unfilledChildren);
        debug(!this->currentBoard->childrenAreFilledIn());
        MYLOG("Needed to fill in children of currentBoard!");
    }
    
    pair<Direction, shared_ptr<const ExpectimaxNodeBase>> bestChild = this->currentBoard->maxChild();
    Direction bestDirection = bestChild.first;
    shared_ptr<const ExpectimaxNodeBase> bestResult = bestChild.second;
    
    pair<unsigned int, ThreesBoard::BoardIndex> addedTileInfo = this->board.move(bestDirection);
    unsigned int addedTileValue = addedTileInfo.first;
    ThreesBoard::BoardIndex addedTileLocation = addedTileInfo.second;
    
    shared_ptr<const ExpectimaxChanceNode> afterMoveBoard = dynamic_pointer_cast<const ExpectimaxChanceNode>(bestResult);
    if (!afterMoveBoard->childrenAreFilledIn()) {
        const_pointer_cast<ExpectimaxChanceNode>(afterMoveBoard)->fillInChildren(this->unfilledChildren);
        debug(!afterMoveBoard->childrenAreFilledIn());
        const_pointer_cast<ExpectimaxChanceNode>(afterMoveBoard)->fillInChildren(this->unfilledChildren);
        MYLOG("Needed to fill in children of afterMoveBoard!");
    }
    
    deque<unsigned int> possibleUpcomingTiles = afterMoveBoard->board.nextTileHint();
    
    shared_ptr<const ExpectimaxNodeBase> baseBoard = afterMoveBoard->child(ChanceNodeEdge(addedTileValue, addedTileLocation));

    shared_ptr<const ExpectimaxMoveNode> afterAddingTileBoard = dynamic_pointer_cast<const ExpectimaxMoveNode>(baseBoard);
    debug(afterAddingTileBoard == nullptr);
    
    this->currentBoard = const_pointer_cast<ExpectimaxMoveNode>(afterAddingTileBoard);
    return bestDirection;
}