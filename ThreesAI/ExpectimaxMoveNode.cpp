//
//  ExpectimaxMoveNode.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxMoveNode.h"

#include "ExpextimaxChanceNode.h"

using namespace std;

ExpectimaxMoveNode::ExpectimaxMoveNode(ThreesBoard const& board): ExpectimaxNode(board) {
    
}

pair<Direction, std::shared_ptr<ExpectimaxNode>> ExpectimaxMoveNode::maxChild() {
    if (!this->childrenAreFilledIn()) {
        return {};
    }
    return *max_element(this->children.begin(), this->children.end(), [](pair<Direction, std::shared_ptr<ExpectimaxNode>> left, pair<Direction, std::shared_ptr<ExpectimaxNode>> right){
        return left.second->value() < right.second->value();
    });
}

bool ExpectimaxMoveNode::childrenAreFilledIn() {
    return !this->children.empty();
}

void ExpectimaxMoveNode::fillInChildren(list<shared_ptr<ExpectimaxNode>> unfilledList, Direction d){
    if (this->childrenAreFilledIn()) {
        return;
    }
    vector<Direction> validMoves = this->board.validMoves();
    for (auto&& d : validMoves) {
        shared_ptr<ExpectimaxChanceNode> child = make_shared<ExpectimaxChanceNode>(this->board);
        this->children.emplace(d, child);
        this->children[d]->board.move(d);
        this->children[d]->fillInChildren(unfilledList, d);
    }
}

unsigned int ExpectimaxMoveNode::value() {
    if (this->board.isGameOver()) {
        return this->board.score();
    }
    if (this->childrenAreFilledIn()) {
        return this->maxChild().second->value();
    }
    return this->board.score();
}

std::shared_ptr<ExpectimaxNode> ExpectimaxMoveNode::child(Direction d) {
    return this->children[d];
}