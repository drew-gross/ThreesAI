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
#include "SimulatedBoardOutput.h"

using namespace std;

ExpectimaxAI::ExpectimaxAI(std::shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output, std::function<float(BoardState const&)> heuristic, unsigned int depth) :
depth(depth),
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
        maxFilledDepth = node->board->hiddenState.numTurns;
        n--;
    }
    unsigned int currentDepth = 0;
    while (currentDepth <= maxFilledDepth && (node = this->nextReachableNode())) {
        if (node->board->hiddenState.numTurns > maxFilledDepth) {
            this->unfilledChildren.push_front(node);
            return;
        }
        node->fillInChildren(this->unfilledChildren);
        currentDepth = node->board->hiddenState.numTurns;
    }
    return;
}

void ExpectimaxAI::fillInToDepth(unsigned int d) {
    shared_ptr<ExpectimaxNodeBase> node;
    while ((node = this->nextReachableNode())) {
        if (node->board->hiddenState.numTurns > d) {
            this->unfilledChildren.push_front(node);
            return;
        }
        node->fillInChildren(this->unfilledChildren);
    }
}

void ExpectimaxAI::prepareDirection() {
    this->fillInToDepth(this->currentState()->hiddenState.numTurns+this->depth);
}

Direction ExpectimaxAI::getDirection() const {
    return this->currentBoard->maxChild(this->heuristic).first;
}

void ExpectimaxAI::setCurrentHint(Hint h) {
    debug(h.contains(Tile::TILE_2) && this->currentBoard->board->hiddenState.twosInStack == 0);
    this->currentBoard->board = make_shared<BoardState const>(BoardState::SetHint(h), *this->currentBoard->board);
    for (auto&& child : this->currentBoard->children) {
        child.second->board = make_shared<BoardState const>(BoardState::SetHint(h), *child.second->board);
    }
    this->currentBoard->pruneUnreachableChildren();
}

void ExpectimaxAI::receiveState(Direction d, BoardState const& afterMoveState) {
    shared_ptr<const ExpectimaxChanceNode> afterMoveBoard = dynamic_pointer_cast<const ExpectimaxChanceNode>(this->currentBoard->child(d));
    AddedTileInfo edge(*afterMoveBoard->board, afterMoveState);
    shared_ptr<const ExpectimaxMoveNode> afterAddingTileBoard = dynamic_pointer_cast<const ExpectimaxMoveNode>(afterMoveBoard->child(edge));
    debug(!afterAddingTileBoard->board->hasSameTilesAs(afterMoveState, {}));
    debug(!(afterMoveState.hiddenState == afterAddingTileBoard->board->hiddenState));
    this->currentBoard = const_pointer_cast<ExpectimaxMoveNode>(afterAddingTileBoard);
    this->setCurrentHint(afterMoveState.getHint());
}
