//
//  ThreesBoardBase.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/16/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesBoardBase.h"

#include <iostream>
#include <iomanip>

using namespace std;

ThreesBoardBase::ThreesBoardBase(array<array<unsigned int, 4>, 4>const&& boardTiles) : board(boardTiles), numTurns(0), isGameOverCache(false), isGameOverCacheIsValid(false), scoreCache(0), scoreCacheIsValid(false) {
    
}

bool ThreesBoardBase::isGameOver() const {
    if (!this->isGameOverCacheIsValid) {
        this->isGameOverCache = this->validMoves().empty();
        this->isGameOverCacheIsValid = true;
    }
    return this->isGameOverCache;
}

unsigned int ThreesBoardBase::at(BoardIndex const& p) const {
    return this->board[p.second][p.first];
}

unsigned int ThreesBoardBase::tileScore(unsigned int tileValue) {
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

unsigned int ThreesBoardBase::score() const {
    if (this->scoreCacheIsValid) {
        return this->scoreCache;
    } else {
        unsigned int result = 0;
        for (auto&& row : this->board) {
            for (auto&& tile : row) {
                result += ThreesBoardBase::tileScore(tile);
            }
        }
        this->scoreCacheIsValid = true;
        this->scoreCache = result;
        return result;
    }
}

bool ThreesBoardBase::canMerge(BoardIndex const& target, BoardIndex const& here) const {
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

bool ThreesBoardBase::canMove(Direction d) const {
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

vector<Direction> ThreesBoardBase::validMoves() const {
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

deque<pair<unsigned int, float>> ThreesBoardBase::possibleNextTiles() const {
    return this->tileStack.possibleNextTiles(this->maxTile());
}

unsigned int ThreesBoardBase::maxTile() const {
    unsigned int maxTile = 0;
    for (array<unsigned int, 4> const& row : this->board) {
        maxTile = max(maxTile, *max_element(row.begin(), row.end()));
    }
    return maxTile;
}

template < class T >
ostream& operator << (ostream& os, const deque<T>& v)
{
    os << "[";
    for (typename deque<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << "]";
    return os;
}

ostream& operator<<(ostream &os, ThreesBoardBase const& board){
    os << "Upcoming: " << board.nextTileHint() << endl;
    if (board.isGameOver()) {
        os << "Final";
    } else {
        os << "Current";
    }
    os << " Score: " <<  board.score() << endl;
    os << "Number of turns: " << board.numTurns << endl;
    os << "---------------------" << endl;
    os << "|" << setw(4) << board.at({0,0}) << "|" << setw(4) << board.at({1,0}) << "|" << setw(4) << board.at({2,0}) << "|" << setw(4) << board.at({3,0}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,1}) << "|" << setw(4) << board.at({1,1}) << "|" << setw(4) << board.at({2,1}) << "|" << setw(4) << board.at({3,1}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,2}) << "|" << setw(4) << board.at({1,2}) << "|" << setw(4) << board.at({2,2}) << "|" << setw(4) << board.at({3,2}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,3}) << "|" << setw(4) << board.at({1,3}) << "|" << setw(4) << board.at({2,3}) << "|" << setw(4) << board.at({3,3}) << "|" << endl;
    os << "---------------------";
    return os;
}