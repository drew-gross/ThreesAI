//
//  ExpectimaxAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxAI.h"

ExpectimaxChanceNode::ExpectimaxChanceNode(ThreesBoard const& board) : board(board){

}

std::pair<Direction, ExpectimaxChanceNode> ExpectimaxMoveNode::maxChild() {
    return *std::max_element(this->children.begin(), this->children.end(), [](std::pair<Direction, ExpectimaxChanceNode> left, std::pair<Direction,ExpectimaxChanceNode> right){
        return left.second.value() < right.second.value();
    });
}

bool ExpectimaxMoveNode::childrenAreFilledIn() {
    return this->children.empty();
}

void ExpectimaxMoveNode::fillInChildren(){
    if (this->childrenAreFilledIn()) {
        return;
    }
    std::vector<Direction> validMoves = this->board.validMoves();
    std::for_each(validMoves.begin(), validMoves.end(), [this](Direction d){
        this->children.emplace(d, this->board);
        this->children[d].board.move(d);
        this->children[d].fillInChildren(d);
    });
}

bool ExpectimaxChanceNode::childrenAreFilledIn() {
    return this->children.empty();
}

void ExpectimaxChanceNode::fillInChildren(Direction d) {
    std::vector<std::pair<float, unsigned int>> possibleIndices;
    std::deque<unsigned int> indices = this->board.possibleUpcomingTiles();
    std::for_each(indices.begin(), indices.end(), [&possibleIndices, &indices](unsigned int tile) {
        possibleIndices.push_back({1.0f/indices.size(), tile});
    });
    auto possibleNextTiles = this->board.possibleUpcomingTiles();
    std::vector<std::pair<float, std::pair<unsigned int, std::stack<unsigned int>>>> possibleNextStacksAndTiles = board.something();
}

unsigned int ExpectimaxMoveNode::value() {
    if (this->board.isGameOver()) {
        return this->board.score();
    }
    if (this->childrenAreFilledIn()) {
        return this->maxChild().second.value();
    }
    return this->board.score();
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

ExpectimaxAI::ExpectimaxAI() : ThreesAIBase() {
    this->currentBoard.fillInChildren();
}

void ExpectimaxAI::playTurn() {
    Direction d = this->currentBoard.maxChild().first;
    std::pair<unsigned int, std::pair<unsigned int, unsigned int>> addedTileInfo = this->board.move(d);
    ExpectimaxChanceNode afterMoveBoard = this->currentBoard.child(d);
    ExpectimaxMoveNode afterAddingTileBoard = afterMoveBoard.child(addedTileInfo);
    this->currentBoard = afterAddingTileBoard;
}