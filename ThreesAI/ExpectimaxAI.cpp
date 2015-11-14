//
//  ExpectimaxAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxAI.h"

#include "Debug.h"
#include "Logging.h"

using namespace std;

ExpectimaxAI::ExpectimaxAI(std::shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output, std::function<float(BoardState const&)> heuristic) :
ThreesAIBase(board, move(output)),
currentBoard(make_shared<ExpectimaxMoveNode>(board, 0)),
heuristic(heuristic){
    this->unfilledChildren.push_back(this->currentBoard);
}

shared_ptr<ExpectimaxNodeBase> ExpectimaxAI::nextReachableNode() {
    weak_ptr<ExpectimaxNodeBase> possibleNode;
    while (possibleNode.expired() && !this->unfilledChildren.empty()) {
        possibleNode = this->unfilledChildren.front();
        this->unfilledChildren.pop_front();
    }
    if (!possibleNode.expired()) {
        return possibleNode.lock();
    }
    return shared_ptr<ExpectimaxNodeBase>();
}

void ExpectimaxAI::fillInChild(unsigned int n) {
    unsigned int maxFilledDepth = 0;
    shared_ptr<ExpectimaxNodeBase> node;
    while (n > 0 && (node = this->nextReachableNode())) {
        node->fillInChildren(this->unfilledChildren);
        maxFilledDepth = node->board->numTurns;
        n--;
    }
    unsigned int currentDepth = 0;
    while (currentDepth <= maxFilledDepth && (node = this->nextReachableNode())) {
        if (node->board->numTurns > maxFilledDepth) {
            this->unfilledChildren.push_front(node);
            return;
        }
        node->fillInChildren(this->unfilledChildren);
        currentDepth = node->board->numTurns;
    }
    return;
}

void ExpectimaxAI::prepareDirection() {
    this->fillInChild(10);
}

Direction ExpectimaxAI::getDirection() const {
    return this->currentBoard->maxChild(this->heuristic).first;
}

void ExpectimaxAI::receiveState(Direction d, BoardState const& afterMoveState) {
    shared_ptr<const ExpectimaxChanceNode> afterMoveBoard = dynamic_pointer_cast<const ExpectimaxChanceNode>(this->currentBoard->child(d));
    debug(afterMoveBoard->board->getHint() != this->currentBoard->board->getHint());
    ChanceNodeEdge edge(afterMoveBoard->board, afterMoveState);
    shared_ptr<const ExpectimaxMoveNode> afterAddingTileBoard = dynamic_pointer_cast<const ExpectimaxMoveNode>(afterMoveBoard->child(edge));
    debug(!afterAddingTileBoard->board->hasSameTilesAs(afterMoveState, {}));
    this->currentBoard = const_pointer_cast<ExpectimaxMoveNode>(afterAddingTileBoard);
    //TODO: get rid of children of current node that can't be reached know that we have a hint
    //this->currentBoard->pruneUnreachableChildren(afterMoveState.getHint(), this->unfilledChildren);
}