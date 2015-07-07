//
//  ThreesBoardBase.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/16/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesBoardBase.h"


#include "Logging.h"

using namespace std;

ThreesBoardBase::ThreesBoardBase(array<unsigned int, 16> boardTiles) : board(boardTiles), numTurns(0), isGameOverCache(false), isGameOverCacheIsValid(false), scoreCache(0), scoreCacheIsValid(false) {
    
}

std::ostream& operator<<(std::ostream &os, ThreesBoardBase const& board){
    os << "Upcoming: " << board.nextTileHint() << std::endl;
    if (board.isGameOver()) {
        os << "Final";
    } else {
        os << "Current";
    }
    os << " Score: " << board.score() << std::endl;
    os << "Number of turns: " << board.numTurns << std::endl;
    os << board.board;
    return os;
}

bool ThreesBoardBase::isGameOver() const {
    if (!this->isGameOverCacheIsValid) {
        this->isGameOverCache = this->validMoves().empty();
        this->isGameOverCacheIsValid = true;
    }
    return this->isGameOverCache;
}

unsigned int ThreesBoardBase::at(BoardIndex const& p) const {
    return this->board[p.first*4+p.second];
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

bool ThreesBoardBase::hasSameTilesAs(ThreesBoardBase const& otherBoard, vector<ThreesBoardBase::BoardIndex> excludedIndices) const {

    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            ThreesBoardBase::BoardIndex curIndex(i,j);
            if (find(excludedIndices.begin(), excludedIndices.end(), curIndex) == excludedIndices.end()) {
                if (this->at(curIndex) != otherBoard.at(curIndex)) {
                    return  false;
                }
            }
        }
    }
    return true;
}

vector<ThreesBoardBase::BoardIndex> ThreesBoardBase::validIndicesForNewTile(Direction movedDirection) const {
    std::array<ThreesBoardBase::BoardIndex, 4> indicies;
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
    auto endIterator = remove_if(indicies.begin(), indicies.end(), [this](BoardIndex tile) {
        return this->at(tile) != 0;
    });
    vector<ThreesBoardBase::BoardIndex> result(indicies.begin(), endIterator);
    return result;
}

unsigned int ThreesBoardBase::score() const {
    if (this->scoreCacheIsValid) {
        return this->scoreCache;
    } else {
        this->scoreCacheIsValid = true;
        this->scoreCache = accumulate(this->board.begin(), this->board.begin(), 0, [](unsigned int acc, unsigned int tile){
            return acc + ThreesBoardBase::tileScore(tile);
        });
        return this->scoreCache;
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
    return *max_element(board.begin(), board.end());
}