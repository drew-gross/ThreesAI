//
//  ExpectimaxAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxAI.h"

std::pair<Direction, ExpectimaxChanceNode> ExpectimaxMoveNode::maxChild() {
    return *std::max_element(this->children.begin(), this->children.end(), [](std::pair<Direction, ExpectimaxChanceNode> left, std::pair<Direction,ExpectimaxChanceNode> right){
        return left.second.value() < right.second.value();
    });
}

unsigned int ExpectimaxMoveNode::value() {
    if (this->board.isGameOver()) {
        return this->board.score();
    }
    return this->maxChild().second.value();
}

unsigned int ExpectimaxChanceNode::value() {
    float value = std::accumulate(this->children.begin(), this->children.end(), 0, [this](float acc, std::pair<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>, ExpectimaxMoveNode> next){
        return acc + this->probability*next.second.value();
    });
    return floor(value);
}

ExpectimaxChanceNode ExpectimaxMoveNode::child(Direction d) {
    return this->children[d];
}

ExpectimaxMoveNode ExpectimaxChanceNode::child(std::pair<unsigned int, std::pair<unsigned int, unsigned int>> t) {
    return this->children[t];
}

void ExpectimaxAI::playTurn() {
    Direction d = this->currentBoard.maxChild().first;
    std::pair<unsigned int, std::pair<unsigned int, unsigned int>> addedTileInfo = this->board.move(d);
    ExpectimaxChanceNode afterMoveBoard = this->currentBoard.child(d);
    ExpectimaxMoveNode afterAddingTileBoard = afterMoveBoard.child(addedTileInfo);
}