//
//  ExpectimaxMoveNode.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxMoveNode.h"

#include "ExpextimaxChanceNode.h"
#include "Debug.h"

using namespace std;

ExpectimaxMoveNode::ExpectimaxMoveNode(ThreesBoard const& board, unsigned int depth): ExpectimaxNode<Direction>(board, depth) {
    
}

pair<Direction, shared_ptr<const ExpectimaxNodeBase>> ExpectimaxMoveNode::maxChild() const {
    debug(!this->childrenAreFilledIn());
    return *max_element(this->children.begin(), this->children.end(), [](pair<Direction, std::shared_ptr<const ExpectimaxNodeBase>> left, pair<Direction, std::shared_ptr<const ExpectimaxNodeBase>> right){
        Direction l = left.first;
        Direction r = right.first;
        float leftValue = left.second->value();
        float rightValue = right.second->value();
        return leftValue < rightValue;
    });
}

void ExpectimaxMoveNode::fillInChildren(list<weak_ptr<ExpectimaxNodeBase>> & unfilledList){
    debug(this->childrenAreFilledIn());
    vector<Direction> validMoves = this->board.validMoves();
    for (auto&& d : validMoves) {
        ThreesBoard childBoard = this->board;
        childBoard.moveWithoutAdd(d);
        shared_ptr<ExpectimaxChanceNode> child = make_shared<ExpectimaxChanceNode>(childBoard, d, this->depth+1);
        this->children.insert({d, child});
        unfilledList.push_back(weak_ptr<ExpectimaxChanceNode>(child));
    }
}

float ExpectimaxMoveNode::value() const {
    if (this->board.isGameOver()) {
        return this->board.score();
    }
    if (this->childrenAreFilledIn()) {
        return this->maxChild().second->value();
    }
    return this->board.score();
}

std::shared_ptr<const ExpectimaxNodeBase> ExpectimaxMoveNode::child(Direction const& d) const {
    auto result = this->children.find(d);
    debug(result == this->children.end());
    return result->second;
}