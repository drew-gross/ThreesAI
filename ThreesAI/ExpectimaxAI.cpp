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

ExpectimaxAI::ExpectimaxAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, Heuristic heuristic, unsigned int depth) :
depth(depth),
ThreesAIBase(board, move(output)),
currentBoard(make_shared<ExpectimaxMoveNode>(board, 0)),
heuristic(heuristic){
    this->boardOutput->doWork = [this](){
        auto node = this->nextReachableNode();
        if (node) {
            node->fillInChildren(this->unfilledChildren);
        }
    };
    this->unfilledChildren.push_back(this->currentBoard);
}

shared_ptr<ExpectimaxNodeBase> ExpectimaxAI::nextReachableNode() {
    weak_ptr<ExpectimaxNodeBase> possibleNode;
    while (possibleNode.expired()) {
        if (this->unfilledChildren.empty()) {
            return shared_ptr<ExpectimaxNodeBase>();
        } else {
            possibleNode = this->unfilledChildren.front();
            this->unfilledChildren.pop_front();
        }
    }
    return possibleNode.lock();
}

void ExpectimaxAI::fillInChild(unsigned int n) {
    unsigned int maxFilledDepth = 0;
    shared_ptr<ExpectimaxNodeBase> node;
    while (n > 0 && (node = this->nextReachableNode())) {
        if (dynamic_pointer_cast<ExpectimaxMoveNode>(node)){
            node->fillInChildren(this->unfilledChildren);
            maxFilledDepth = dynamic_pointer_cast<ExpectimaxMoveNode>(node)->board->hiddenState.numTurns;
            n--;
        } else {
            node->fillInChildren(this->unfilledChildren);
            maxFilledDepth = dynamic_pointer_cast<ExpectimaxChanceNode>(node)->board->hiddenState.numTurns;
            n--;
        }
    }
    unsigned int currentDepth = 0;
    while (currentDepth <= maxFilledDepth && (node = this->nextReachableNode())) {
        if (dynamic_pointer_cast<ExpectimaxMoveNode>(node)){
            if (dynamic_pointer_cast<ExpectimaxMoveNode>(node)->board->hiddenState.numTurns > maxFilledDepth) {
                this->unfilledChildren.push_front(node);
                return;
            }
        } else {
            if (dynamic_pointer_cast<ExpectimaxChanceNode>(node)->board->hiddenState.numTurns > maxFilledDepth) {
                this->unfilledChildren.push_front(node);
                return;
            }
        }
        node->fillInChildren(this->unfilledChildren);
        if (dynamic_pointer_cast<ExpectimaxMoveNode>(node)){
            currentDepth = dynamic_pointer_cast<ExpectimaxMoveNode>(node)->board->hiddenState.numTurns;
        } else {
            currentDepth = dynamic_pointer_cast<ExpectimaxChanceNode>(node)->board->hiddenState.numTurns;
        }
    }
    return;
}

void ExpectimaxAI::fillInToDepth(unsigned int d) {
    shared_ptr<ExpectimaxNodeBase> node;
    while ((node = this->nextReachableNode())) {
        if (dynamic_pointer_cast<ExpectimaxMoveNode>(node)){
            if (dynamic_pointer_cast<ExpectimaxMoveNode>(node)->board->hiddenState.numTurns > d) {
                this->unfilledChildren.push_front(node);
                break;
            } else {
                node->fillInChildren(this->unfilledChildren);
            }
        } else {
            if (dynamic_pointer_cast<ExpectimaxChanceNode>(node)->board->hiddenState.numTurns > d) {
                this->unfilledChildren.push_front(node);
                break;
            } else {
                node->fillInChildren(this->unfilledChildren);
            }
        }
    }
    //TODO: this part should maybe more into a different function
    int fillInFurtherThreshold = 0;
    switch (this->depth) {
        case 1:
            fillInFurtherThreshold = 200;
            break;
        case 2:
            fillInFurtherThreshold = 20000;
            break;
        case 3:
            fillInFurtherThreshold = 500000;
            break;
    }
    if (this->unfilledChildren.size() < fillInFurtherThreshold && !this->unfilledChildren.empty()) {
        this->fillInToDepth(d+1);
    }
}

void ExpectimaxAI::prepareDirection() {
    this->fillInToDepth(this->currentState()->hiddenState.numTurns+this->depth);
}

Direction ExpectimaxAI::getDirection() const {
    const ExpectimaxAI *self = this;
    return this->currentBoard->maxChild([self](AboutToMoveBoard const&b){return self->heuristic.evaluateWithoutDescription(b);}).first;
}

void ExpectimaxAI::setCurrentHint(Hint h) {
    this->currentBoard->board = make_shared<AboutToMoveBoard const>(AboutToMoveBoard::SetHint(h), *this->currentBoard->board);
    for (auto&& child : this->currentBoard->children) {
        dynamic_pointer_cast<ExpectimaxChanceNode>(child.second)->board = make_shared<AboutToAddTileBoard const>(SetHint(h), *dynamic_pointer_cast<ExpectimaxChanceNode const>(child.second)->board);
    }
    this->currentBoard->pruneUnreachableChildren();
}

void ExpectimaxAI::receiveState(Direction d, AboutToMoveBoard const& measuredState) {
    auto chanceChild = this->currentBoard->child(d);
    if (chanceChild == nullptr) {
        MYLOG(chanceChild);
        MYLOG(d);
        MYLOG(this->currentState());
        MYLOG(measuredState);
    }
    ChanceTreePtr afterMoveTree = dynamic_pointer_cast<const ExpectimaxChanceNode>(chanceChild);
    AddedTileInfo edge(*afterMoveTree->board, measuredState);
    
    auto moveChild = afterMoveTree->child(edge);
    if (moveChild == nullptr) {
        MYLOG(moveChild);
        MYLOG(edge);
        MYLOG(this->currentState());
        MYLOG(measuredState);
    }
    MoveTreePtr afterTileTree = dynamic_pointer_cast<const ExpectimaxMoveNode>(afterMoveTree->child(edge));
    this->currentBoard = const_pointer_cast<ExpectimaxMoveNode>(afterTileTree);
    this->setCurrentHint(measuredState.getHint());
}
