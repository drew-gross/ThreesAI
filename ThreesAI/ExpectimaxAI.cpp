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

using namespace std;

ExpectimaxAI::ExpectimaxAI() : ThreesAIBase() {
    this->currentBoard = make_shared<ExpectimaxMoveNode>(this->board);
    this->unfilledChildren.push_back(this->currentBoard);
}

void ExpectimaxAI::fillInChild() {
    shared_ptr<ExpectimaxNodeBase> child = this->unfilledChildren.front();
    child->fillInChildren(this->unfilledChildren);
    this->unfilledChildren.pop_front();
}

Direction ExpectimaxAI::playTurn() {
    clock_t analysisStartTime = clock();
    
    while (float(clock() - analysisStartTime)/CLOCKS_PER_SEC < .001) {
        this->fillInChild();
    }
    
    pair<Direction, shared_ptr<ExpectimaxNodeBase>> bestChild = this->currentBoard->maxChild();
    Direction bestDirection = bestChild.first;
    shared_ptr<ExpectimaxNodeBase> bestResult = bestChild.second;
    
    pair<unsigned int, ThreesBoard::BoardIndex> addedTileInfo = this->board.move(bestDirection);
    unsigned int addedTileValue = addedTileInfo.first;
    ThreesBoard::BoardIndex addedTileLocation = addedTileInfo.second;
    
    shared_ptr<ExpectimaxChanceNode> afterMoveBoard = dynamic_pointer_cast<ExpectimaxChanceNode>(this->currentBoard->child(bestDirection));
    debug(afterMoveBoard == nullptr);
    
    deque<unsigned int> possibleUpcomingTiles = afterMoveBoard->board.possibleUpcomingTiles();
    shared_ptr<ExpectimaxNodeBase> baseBoard = afterMoveBoard->child(ChanceNodeEdge(addedTileValue, addedTileLocation, possibleUpcomingTiles.front()));
    debug(baseBoard == nullptr);
    afterMoveBoard->child(ChanceNodeEdge(addedTileValue, addedTileLocation, possibleUpcomingTiles.front()));
    shared_ptr<ExpectimaxMoveNode> afterAddingTileBoard = dynamic_pointer_cast<ExpectimaxMoveNode>(baseBoard);
    debug(afterAddingTileBoard == nullptr);
    
    this->currentBoard = afterAddingTileBoard;
    return bestDirection;
}