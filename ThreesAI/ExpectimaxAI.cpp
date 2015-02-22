//
//  ExpectimaxAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxAI.h"

#include <time.h>

ExpectimaxMoveNode::ExpectimaxMoveNode() {
    
}

ExpectimaxChanceNode::ExpectimaxChanceNode(ThreesBoard const& board) : board(board){

}

ExpectimaxMoveNode::ExpectimaxMoveNode(ThreesBoard const& board): board(board) {
    
}

std::pair<Direction, ExpectimaxChanceNode> ExpectimaxMoveNode::maxChild() {
    if (!this->childrenAreFilledIn()) {
        return {};
    }
    return *std::max_element(this->children.begin(), this->children.end(), [](std::pair<Direction, ExpectimaxChanceNode> left, std::pair<Direction,ExpectimaxChanceNode> right){
        return left.second.value() < right.second.value();
    });
}

bool ExpectimaxMoveNode::childrenAreFilledIn() {
    return !this->children.empty();
}

void ExpectimaxMoveNode::fillInChildren(std::list<ExpectimaxNode*>& unfilledList, Direction d){
    if (this->childrenAreFilledIn()) {
        return;
    }
    std::vector<Direction> validMoves = this->board.validMoves();
    std::for_each(validMoves.begin(), validMoves.end(), [this, &unfilledList](Direction d){
        this->children.emplace(d, this->board);
        this->children[d].board.move(d);
        this->children[d].fillInChildren(unfilledList, d);
    });
}

bool ExpectimaxChanceNode::childrenAreFilledIn() {
    return this->children.empty();
}

void ExpectimaxChanceNode::fillInChildren(std::list<ExpectimaxNode*>& unfilledList, Direction d) {
    auto possibleNextTiles = this->board.possibleUpcomingTiles();
    auto possibleNextLocations = this->board.validIndicesForNewTile(d);
    std::vector<std::tuple<float, ThreesBoard>> possibleNextBoardStates = this->board.possibleNextBoardStates();
    
    float tileProbability = 1.0f/possibleNextLocations.size();
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    std::for_each(possibleNextTiles.begin(), possibleNextTiles.end(), [&](unsigned int possibleTile){
        std::for_each(possibleNextLocations.begin(), possibleNextLocations.end(), [&](ThreesBoard::BoardIndex boardIndex){
            std::for_each(possibleNextBoardStates.begin(), possibleNextBoardStates.end(), [&](std::tuple<float, ThreesBoard> state){
                float stateProbability = tileProbability*locationProbability*std::get<0>(state);
                
                std::pair<unsigned int, ThreesBoard::BoardIndex> childIndex = {possibleTile, boardIndex};
                ThreesBoard childState = std::get<1>(state);
                this->children.insert({childIndex, {stateProbability, childState}});
                *this->child({possibleTile, boardIndex}).second.board.at(boardIndex) = possibleTile;
                unfilledList.push_back(&this->child({possibleTile, boardIndex}).second);
            });
        });
    });
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
    float value = std::accumulate(this->children.begin(), this->children.end(), 0, [this](float acc, std::pair<std::pair<unsigned int, ThreesBoard::BoardIndex>, std::pair<float,ExpectimaxMoveNode>> next){
        return acc + next.second.first*next.second.second.value();
    });
    return floor(value);
}

ExpectimaxChanceNode& ExpectimaxMoveNode::child(Direction d) {
    return this->children[d];
}

std::pair<float, ExpectimaxMoveNode>& ExpectimaxChanceNode::child(std::pair<unsigned int, ThreesBoard::BoardIndex> t) {
    return this->children[t];
}

ExpectimaxAI::ExpectimaxAI() : ThreesAIBase() , currentBoard(board) {
    this->unfilledChildren.push_back(&this->currentBoard);
}

void ExpectimaxAI::fillInChild() {
    ExpectimaxNode *child = this->unfilledChildren.front();
    this->unfilledChildren.pop_front();
    child->fillInChildren(this->unfilledChildren, UP);
}

void ExpectimaxAI::playTurn() {
    clock_t analysisStartTime = clock();
    
    bool keepCalculating = true;
    while (keepCalculating) {
        keepCalculating = float(clock() - analysisStartTime)/CLOCKS_PER_SEC < 1;
        this->fillInChild();
    }
    
    Direction d = this->currentBoard.maxChild().first;
    std::pair<unsigned int, ThreesBoard::BoardIndex> addedTileInfo = this->board.move(d);
    ExpectimaxChanceNode afterMoveBoard = this->currentBoard.child(d);
    ExpectimaxMoveNode afterAddingTileBoard = afterMoveBoard.child(addedTileInfo).second;
    this->currentBoard = afterAddingTileBoard;
}