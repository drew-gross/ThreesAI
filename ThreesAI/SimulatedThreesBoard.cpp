//
//  ThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "SimulatedThreesBoard.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <exception>

#include "Logging.h"
#include "Debug.h"

using namespace std;

class InvalidTileAdditionException : public runtime_error {
public:
    InvalidTileAdditionException() : runtime_error("Attempting to add a tile where none can be added"){};
};

SimulatedThreesBoard SimulatedThreesBoard::randomBoard() {
    array<unsigned int, 16> initialTiles = {3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0};
    shuffle(initialTiles.begin(), initialTiles.end(), TileStack::randomGenerator);
    array<array<unsigned int, 4>, 4> initialBoard = array<array<unsigned int, 4>, 4>();
    for (unsigned i = 0; i < initialTiles.size(); i++) {
        initialBoard[i/4][i%4] = initialTiles[i];
    }
    SimulatedThreesBoard simBoard(std::move(initialBoard));
    return simBoard;
}

SimulatedThreesBoard::SimulatedThreesBoard(array<array<unsigned int, 4>, 4>const&& otherBoard) : ThreesBoardBase(std::move(otherBoard)) {
}

void SimulatedThreesBoard::set(BoardIndex const& p, const unsigned int t){
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    
    this->board[p.second][p.first] = t;
}

SimulatedThreesBoard SimulatedThreesBoard::simulatedCopy() const {
    return SimulatedThreesBoard(*this);
}

bool SimulatedThreesBoard::tryMerge(BoardIndex const& target, BoardIndex const& other) {
    if (this->canMerge(target, other)) {
        this->set(target, this->at(target) + this->at(other));
        this->set(other, 0);
        
        this->isGameOverCacheIsValid = false;
        this->scoreCacheIsValid = false;
        return true;
    } else {
        return false;
    }
}

bool SimulatedThreesBoard::moveWithoutAdd(Direction d) {
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    
    bool successfulMerge = false;
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({i, 0}, {i, 1});
                successfulMerge |= this->tryMerge({i, 1}, {i, 2});
                successfulMerge |= this->tryMerge({i, 2}, {i, 3});
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({i, 3}, {i, 2});
                successfulMerge |= this->tryMerge({i, 2}, {i, 1});
                successfulMerge |= this->tryMerge({i, 1}, {i, 0});
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({0, i}, {1, i});
                successfulMerge |= this->tryMerge({1, i}, {2, i});
                successfulMerge |= this->tryMerge({2, i}, {3, i});
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({3, i}, {2, i});
                successfulMerge |= this->tryMerge({2, i}, {1, i});
                successfulMerge |= this->tryMerge({1, i}, {0, i});
            }
            break;
        default:
            break;
    }
    if (successfulMerge) {
        this->numTurns++;
    }
    return successfulMerge;
}

pair<unsigned int, SimulatedThreesBoard::BoardIndex> SimulatedThreesBoard::move(Direction d) {
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    
    if (this->moveWithoutAdd(d)) {
        return this->addTile(d);
    }
    throw InvalidMoveException();
}

deque<unsigned int> SimulatedThreesBoard::nextTileHint() const {
    return this->tileStack.nextTileHint(this->maxTile());
}

vector<SimulatedThreesBoard::BoardIndex> SimulatedThreesBoard::validIndicesForNewTile(Direction movedDirection) const {
    array<BoardIndex, 4> indicies;
    switch (movedDirection) {
        case LEFT:
            indicies = {BoardIndex(3,0),BoardIndex(3,1),BoardIndex(3,2),BoardIndex(3,3)};
            break;
        case RIGHT:
            indicies = {BoardIndex(0,0),BoardIndex(0,1),BoardIndex(0,2),BoardIndex(0,3)};
            break;
        case UP:
            indicies = {BoardIndex(0,3),BoardIndex(1,3),BoardIndex(2,3),BoardIndex(3,3)};
            break;
        case DOWN:
            indicies = {BoardIndex(0,0),BoardIndex(1,0),BoardIndex(2,0),BoardIndex(3,0)};
            break;
        default:
            break;
    }
    auto endIterator = remove_if(indicies.begin(), indicies.end(), [this](SimulatedThreesBoard::BoardIndex tile) {
        return this->at(tile) != 0;
    });
    vector<SimulatedThreesBoard::BoardIndex> result(indicies.begin(), endIterator);
    return result;
}

pair<unsigned int, SimulatedThreesBoard::BoardIndex> SimulatedThreesBoard::addTile(Direction d) {
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    
    auto indices = this->validIndicesForNewTile(d);
    shuffle(indices.begin(), indices.end(), TileStack::randomGenerator);
    unsigned int nextTileValue = this->tileStack.getNextTile(this->maxTile());
    this->set(*indices.begin(), nextTileValue);
    return {nextTileValue, *indices.begin()};
}

ostream& operator<<(ostream &os, Direction d){
    switch (d) {
        case UP:
            os << "UP";
            break;
        case DOWN:
            os << "DOWN";
            break;
        case RIGHT:
            os << "RIGHT";
            break;
        case LEFT:
            os << "LEFT";
            break;
    }
    return os;
}

ostream& operator<<(ostream &os, const SimulatedThreesBoard::BoardIndex i){
    os << "{" << i.first << ", " << i.second << "}";
    return os;
}