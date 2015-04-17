//
//  ThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesBoard.h"
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

ThreesBoard::ThreesBoard() : ThreesBoardBase(), isGameOverCache(false), isGameOverCacheIsValid(false), scoreCache(0), scoreCacheIsValid(false) {
    array<unsigned int, 16> initialTiles = {3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0};
    shuffle(initialTiles.begin(), initialTiles.end(), TileStack::randomGenerator);
    this->board = array<array<unsigned int, 4>, 4>();
    for (unsigned i = 0; i < initialTiles.size(); i++) {
        this->board[i/4][i%4] = initialTiles[i];
    }
}

void ThreesBoard::set(BoardIndex const& p, const unsigned int t){
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    
    this->board[p.second][p.first] = t;
}

unsigned int ThreesBoard::at(BoardIndex const& p) const {
    return this->board[p.second][p.first];
}

ThreesBoard ThreesBoard::simulatedCopy() {
    return ThreesBoard(*this);
}

bool ThreesBoard::canMerge(BoardIndex const& target, BoardIndex const& here) const {
    if (this->at(here) == 0) {
        return false;
    }
    if (this->at(target) == 0 and this->at(here) != 0) {
        return true;
    }
    if (this->at(target) == this->at(here) && this->at(target) != 1 && this->at(target) != 2) {
        return true;
    }
    if ((this->at(target) == 1 and this->at(here) == 2) or (this->at(target) == 2 and this->at(here) == 1)) {
        return true;
    }
    return false;
}

bool ThreesBoard::tryMerge(BoardIndex const& target, BoardIndex const& other) {
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

bool ThreesBoard::canMove(Direction d) const {
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({i, 0}, {i, 1})) {
                    return true;
                }
                if (this->canMerge({i, 1}, {i, 2})) {
                    return true;
                }
                if (this->canMerge({i, 2}, {i, 3})) {
                    return true;
                }
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({i, 3}, {i, 2})) {
                    return true;
                }
                if (this->canMerge({i, 2}, {i, 1})) {
                    return true;
                }
                if (this->canMerge({i, 1}, {i, 0})) {
                    return true;
                }
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({0, i}, {1, i})) {
                    return true;
                }
                if (this->canMerge({1, i}, {2, i})) {
                    return true;
                }
                if (this->canMerge({2, i}, {3, i})) {
                    return true;
                }
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({3, i}, {2, i})) {
                    return true;
                }
                if (this->canMerge({2, i}, {1, i})) {
                    return true;
                }
                if (this->canMerge({1, i}, {0, i})) {
                    return true;
                }
            }
            break;
            
        default:
            break;
    }
    return false;
}

bool ThreesBoard::moveWithoutAdd(Direction d) {
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

pair<unsigned int, ThreesBoard::BoardIndex> ThreesBoard::move(Direction d) {
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    
    if (this->moveWithoutAdd(d)) {
        return this->addTile(d);
    }
    throw InvalidMoveException();
}

unsigned int ThreesBoard::maxTile() const {
    unsigned int maxTile = 0;
    for (array<unsigned int, 4> const& row : this->board) {
        maxTile = max(maxTile, *max_element(row.begin(), row.end()));
    }
    return maxTile;
}

vector<ThreesBoard::BoardIndex> ThreesBoard::validIndicesForNewTile(Direction movedDirection) const {
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
    auto endIterator = remove_if(indicies.begin(), indicies.end(), [this](ThreesBoard::BoardIndex tile) {
        return this->at(tile) != 0;
    });
    vector<ThreesBoard::BoardIndex> result(indicies.begin(), endIterator);
    return result;
}

pair<unsigned int, ThreesBoard::BoardIndex> ThreesBoard::addTile(Direction d) {
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    
    auto indices = this->validIndicesForNewTile(d);
    shuffle(indices.begin(), indices.end(), TileStack::randomGenerator);
    unsigned int nextTileValue = this->tileStack.getNextTile(this->maxTile());
    this->set(*indices.begin(), nextTileValue);
    return {nextTileValue, *indices.begin()};
}

unsigned int ThreesBoard::score() const {
    if (this->scoreCacheIsValid) {
        return this->scoreCache;
    } else {
        unsigned int result = 0;
        for (auto&& row : this->board) {
            for (auto&& tile : row) {
                result += ThreesBoard::tileScore(tile);
            }
        }
        this->scoreCacheIsValid = true;
        this->scoreCache = result;
        return result;
    }
}

unsigned int ThreesBoard::tileScore(unsigned int tileValue) {
    switch (tileValue) {
        case 0: return 0;
        case 1: return 0;
        case 2: return 0;
        default:
            unsigned int result = 3;
            tileValue /= 3;
            while (tileValue > 1) {
                result *= 3;
                tileValue /= 2;
            }
            return result;
    }
}

vector<Direction> ThreesBoard::validMoves() const {
    vector<Direction> result;
    result.reserve(4);
    if (this->canMove(DOWN)) {
        result.push_back(DOWN);
    }
    if (this->canMove(UP)) {
        result.push_back(UP);
    }
    if (this->canMove(LEFT)) {
        result.push_back(LEFT);
    }
    if (this->canMove(RIGHT)) {
        result.push_back(RIGHT);
    }
    return result;
}

bool ThreesBoard::isGameOver() const {
    if (!this->isGameOverCacheIsValid) {
        this->isGameOverCache = this->validMoves().empty();
        this->isGameOverCacheIsValid = true;
    }
    return this->isGameOverCache;
}

deque<unsigned int> ThreesBoard::nextTileHint() const {
    return this->tileStack.nextTileHint(this->maxTile());
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

ostream& operator<<(ostream &os, const ThreesBoard::BoardIndex i){
    os << "{" << i.first << ", " << i.second << "}";
    return os;
}