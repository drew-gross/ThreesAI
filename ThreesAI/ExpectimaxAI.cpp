//
//  ExpectimaxAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxAI.h"

#include <time.h>

using namespace std;

ExpectimaxMoveNode::ExpectimaxMoveNode() {
    
}

ExpectimaxChanceNode::ExpectimaxChanceNode(ThreesBoard const& board) : board(board){

}

ExpectimaxMoveNode::ExpectimaxMoveNode(ThreesBoard const& board): board(board) {
    
}

pair<Direction, ExpectimaxChanceNode> ExpectimaxMoveNode::maxChild() {
    if (!this->childrenAreFilledIn()) {
        return {};
    }
    return *max_element(this->children.begin(), this->children.end(), [](pair<Direction, ExpectimaxChanceNode> left, pair<Direction,ExpectimaxChanceNode> right){
        return left.second.value() < right.second.value();
    });
}

bool ExpectimaxMoveNode::childrenAreFilledIn() {
    return !this->children.empty();
}

void ExpectimaxMoveNode::fillInChildren(list<ExpectimaxNode*>& unfilledList, Direction d){
    if (this->childrenAreFilledIn()) {
        return;
    }
    vector<Direction> validMoves = this->board.validMoves();
    for (auto&& d : validMoves) {
        this->children.emplace(d, this->board);
        this->children[d].board.move(d);
        this->children[d].fillInChildren(unfilledList, d);
    }
}

bool ExpectimaxChanceNode::childrenAreFilledIn() {
    return this->children.empty();
}

void ExpectimaxChanceNode::fillInChildren(list<ExpectimaxNode*>& unfilledList, Direction d) {
    auto possibleNextTiles = this->board.tileStack.possibleUpcomingTiles(this->board.maxTile());
    auto possibleNextLocations = this->board.validIndicesForNewTile(d);
    vector<tuple<float, ThreesBoard, unsigned int>> possibleNextBoardStates = this->board.possibleNextBoardStates();
    
    float tileProbability = 1.0f/possibleNextLocations.size();
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& possibleTile : possibleNextTiles) {
        for (auto&& boardIndex : possibleNextLocations) {
            for (auto&& state : possibleNextBoardStates) {
                float stateProbability = tileProbability*locationProbability*get<0>(state);
                ThreesBoard childState = get<1>(state);
                unsigned int upcomingTile = get<2>(state);
                tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int> childIndex = {possibleTile, boardIndex, upcomingTile};
                this->children.insert({childIndex, {stateProbability, childState}});
                this->child(childIndex).second.board.set(boardIndex, possibleTile);
                unfilledList.push_back(&this->child(childIndex).second);
            }
        }
    }
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
    float value = accumulate(this->children.begin(), this->children.end(), 0, [this](float acc, pair<tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int>, pair<float,ExpectimaxMoveNode>> next){
        return acc + next.second.first*next.second.second.value();
    });
    return floor(value);
}

ExpectimaxChanceNode& ExpectimaxMoveNode::child(Direction d) {
    return this->children[d];
}

pair<float, ExpectimaxMoveNode>& ExpectimaxChanceNode::child(tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int> t) {
    return this->children[t];
}

ExpectimaxAI::ExpectimaxAI() : ThreesAIBase() {
    this->currentBoard = ExpectimaxMoveNode(this->board);
    this->unfilledChildren.push_back(&this->currentBoard);
}

void ExpectimaxAI::fillInChild() {
    ExpectimaxNode *child = this->unfilledChildren.front();
    child->fillInChildren(this->unfilledChildren, UP);
    this->unfilledChildren.pop_front();
}

void ExpectimaxAI::playTurn() {
    clock_t analysisStartTime = clock();
    
    float keepCalculating;
    while (keepCalculating < .0001) {
        keepCalculating = float(clock() - analysisStartTime)/CLOCKS_PER_SEC;
        this->fillInChild();
    }
    
    Direction d = this->currentBoard.maxChild().first;
    pair<unsigned int, ThreesBoard::BoardIndex> addedTileInfo = this->board.move(d);
    unsigned int addedTileValue = addedTileInfo.first;
    ThreesBoard::BoardIndex addedTileLocation = addedTileInfo.second;
    ExpectimaxChanceNode afterMoveBoard = this->currentBoard.child(d);
    ExpectimaxMoveNode afterAddingTileBoard = afterMoveBoard.child({addedTileValue, addedTileLocation, afterMoveBoard.board.tileStack.possibleUpcomingTiles(afterMoveBoard.board.maxTile()).front()}).second;
    this->currentBoard = afterAddingTileBoard;
}