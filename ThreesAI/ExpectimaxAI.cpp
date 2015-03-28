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
    unsigned int maxFilledDepth = 0;
    while (n > 0) {
        weak_ptr<ExpectimaxNodeBase> possibleNode;
        while (possibleNode.expired()) {
            if (this->unfilledChildren.empty()) {
                return;
            }
            possibleNode = this->unfilledChildren.front();
            this->unfilledChildren.pop_front();
        }
        shared_ptr<ExpectimaxNodeBase> node = possibleNode.lock();
        node->fillInChildren(this->unfilledChildren);
        maxFilledDepth = node->board.numTurns;
        n--;
    }
    unsigned int currentDepth = 0;
    while (currentDepth <= maxFilledDepth) {
        weak_ptr<ExpectimaxNodeBase> possibleNode;
        while (possibleNode.expired()) {
            if (this->unfilledChildren.empty()) {
                return;
            }
            possibleNode = this->unfilledChildren.front();
            this->unfilledChildren.pop_front();
        }
        shared_ptr<ExpectimaxNodeBase> node = possibleNode.lock();
        if (node->board.numTurns > maxFilledDepth) {
            this->unfilledChildren.push_front(node);
            return;
        }
        node->fillInChildren(this->unfilledChildren);
        currentDepth = node->board.numTurns;
    }
    return;
}

Direction ExpectimaxAI::playTurn() {
    this->currentBoard->pruneUnreachableChildren(this->board.nextTileHint());
    this->fillInChild(40);
    
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