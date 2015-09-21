//
//  BoardState.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "BoardState.h"

#include "InvalidMoveException.h"

#include <iostream>
#include <boost/algorithm/string.hpp>

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace boost;

BoardState::BoardState(Board b,
                       unsigned int numTurns,
                       cv::Mat sourceImage,
                       unsigned int onesInStack,
                       unsigned int twosInStack,
                       unsigned int threesInStack,
                       std::deque<unsigned int> hint) :
board(b),
isGameOverCacheIsValid(false),
scoreCacheIsValid(false),
numTurns(numTurns),
onesInStack(onesInStack),
twosInStack(twosInStack),
threesInStack(threesInStack),
forcedHint(hint),
sourceImage(sourceImage)
{}

array<BoardState::BoardIndex, 16> BoardState::indexes() {
    static bool initialized = false;
    static array<BoardIndex, 16> results;
    if (!initialized) {
        for (int i = 0; i < 16; i++) {
            results[i] = {i/4,i%4};
        }
    }
    return results;
}

ostream& operator<<(ostream &os, const BoardState::BoardIndex i){
    os << "{" << i.first << ", " << i.second << "}";
    return os;
}

BoardState BoardState::fromString(const string s) {
    
    vector<string> splitName;
    split(splitName, s, is_any_of("-"));
    
    deque<string> nums;
    split(nums, splitName[0], is_any_of(","));
    debug(nums.size() != 16);
    
    deque<string> nextTileHintStrings;
    split(nextTileHintStrings, splitName[1], is_any_of(","));
    debug(nextTileHintStrings.size() > 3);
    
    std::array<unsigned int, 16> tileList;
    transform(nums.begin(), nums.end(), tileList.begin(), [](string s){
        return stoi(s);
    });
    
    deque<unsigned int> hint(nextTileHintStrings.size());
    transform(nextTileHintStrings.begin(), nextTileHintStrings.end(), hint.begin(), [](string s) {
        return stoi(s);
    });
    
    //TODO: get numTurns and source image and values in stack from somewhere
    return BoardState(tileList, 0, cv::Mat(), 4, 4, 4, hint);
}

unsigned int BoardState::at(BoardIndex const& p) const {
    return this->board[p.first+p.second*4];
}

bool BoardState::isGameOver() const {
    if (!this->isGameOverCacheIsValid) {
        this->isGameOverCache = this->validMoves().empty();
        this->isGameOverCacheIsValid = true;
    }
    return this->isGameOverCache;
}

bool BoardState::canMerge(BoardIndex const& target, BoardIndex const& here) const {
    if (this->at(target) == 0) {
        return this->at(here) != 0;
    }
    if (this->at(target) == 1) {
        return this->at(here) == 2;
    }
    if (this->at(target) == 2) {
        return this->at(here) == 1;
    }
    return this->at(target) == this->at(here);
}

bool BoardState::canMove(Direction d) const {
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({i, 0}, {i, 1}) ||
                    this->canMerge({i, 1}, {i, 2}) ||
                    this->canMerge({i, 2}, {i, 3})) {
                    return true;
                }
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({i, 3}, {i, 2}) ||
                    this->canMerge({i, 2}, {i, 1}) ||
                    this->canMerge({i, 1}, {i, 0})) {
                    return true;
                }
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({0, i}, {1, i}) ||
                    this->canMerge({1, i}, {2, i}) ||
                    this->canMerge({2, i}, {3, i})) {
                    return true;
                }
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({3, i}, {2, i}) ||
                    this->canMerge({2, i}, {1, i}) ||
                    this->canMerge({1, i}, {0, i})) {
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

unsigned int BoardState::upcomingTile() const {
    default_random_engine genCopy = this->generator;
    uniform_real_distribution<> r(0,1);
    float tileFinder = r(genCopy);
    auto possibleUpcomingTiles = this->possibleNextTiles();
    for (auto&& possibleUpcomingTile : possibleUpcomingTiles) {
        float p = possibleUpcomingTile.second;
        unsigned int tile = possibleUpcomingTile.first;
        if (tileFinder < p) {
            return tile;
        } else {
            tileFinder -= p;
        }
    }
    debug();
    return 0;
}

unsigned int BoardState::maxBonusTile() const {
    return this->maxTile()/8;
}

BoardState::Hint BoardState::nextTileHint() const {
    if (this->forcedHint.size() != 0) {
        return this->forcedHint;
    }
    deque<unsigned int> inRangeTiles;
    unsigned int actualTile = this->upcomingTile();
    default_random_engine genCopy = this->generator;
    if (actualTile <= 3) {
        inRangeTiles.push_back(actualTile);
    } else {
        //bonus tile
        
        //add possible values to the list
        if (actualTile / 4 >= 6) {
            inRangeTiles.push_back(actualTile/4);
        }
        if (actualTile / 2 >= 6) {
            inRangeTiles.push_back(actualTile/2);
        }
        inRangeTiles.push_back(actualTile);
        if (actualTile * 2 <= this->maxBonusTile()) {
            inRangeTiles.push_back(actualTile*2);
        }
        if (actualTile * 4 <= this->maxBonusTile()) {
            inRangeTiles.push_back(actualTile*4);
        }
        
        //trim the list down to size
        if (inRangeTiles.size() <= 3) {
            return inRangeTiles;
        }
        if (inRangeTiles.size() == 4) {
            if (uniform_int_distribution<>(0,1)(genCopy) == 1) {
                inRangeTiles.pop_back();
            } else {
                inRangeTiles.pop_front();
            }
        } else {
            int rand = uniform_int_distribution<>(0,2)(genCopy);
            if (rand == 0) {
                inRangeTiles.pop_back();
                inRangeTiles.pop_back();
            } else if (rand == 1) {
                inRangeTiles.pop_back();
                inRangeTiles.pop_front();
            } else {
                inRangeTiles.pop_front();
                inRangeTiles.pop_front();
            }
        }
    }
    return inRangeTiles;
};

bool BoardState::hasSameTilesAs(BoardState const& otherBoard, vector<BoardState::BoardIndex> excludedIndices) const {
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            BoardIndex curIndex(i,j);
            if (find(excludedIndices.begin(), excludedIndices.end(), curIndex) == excludedIndices.end()) {
                unsigned int tile = this->at(curIndex);
                unsigned int otherTile = otherBoard.at(curIndex);
                if (tile != otherTile) {
                    return false;
                }
            }
        }
    }
    return true;
}

vector<BoardState::BoardIndex> BoardState::validIndicesForNewTile(Direction movedDirection) const {
    std::array<BoardIndex, 4> indicies;
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
    vector<BoardIndex> result;
    for (auto&& index : indicies) {
        if (this->at(index) == 0) {
            result.push_back(index);
        }
    }
    return result;
}

unsigned int BoardState::stackSize() const {
    return this->onesInStack + this->twosInStack + this->threesInStack;
}

float BoardState::nonBonusTileProbability(unsigned int tile, bool canHaveBonus) const {
    unsigned int count = 0;
    switch (tile) {
        case 1:
            count = this->onesInStack;
            break;
        case 2:
            count = this->twosInStack;
            break;
        case 3:
            count = this->threesInStack;
            break;
    }
    float nonBonusProbability = float(count)/this->stackSize();
    if (canHaveBonus) {
        nonBonusProbability *= float(20)/21;
    }
    return nonBonusProbability;
}

unsigned int int_log2(unsigned int x) {
    unsigned int result = 0;
    while (x > 0) {
        result++;
        x >>= 1;
    }
    return result;
}

deque<pair<unsigned int, float>> BoardState::possibleNextTiles() const {
    unsigned int maxBoardTile = this->maxTile();
    bool canHaveBonus = this->maxTile() >= 48;
    deque<pair<unsigned int, float>> result;
    //should be able to only add 1,2,3 if they are in the stack
    if (this->onesInStack > 0) {
        result.push_back({1, this->nonBonusTileProbability(1, canHaveBonus)});
    }
    if (this->twosInStack > 0) {
        result.push_back({2, this->nonBonusTileProbability(2, canHaveBonus)});
    }
    if (this->threesInStack > 0) {
        result.push_back({3, this->nonBonusTileProbability(3, canHaveBonus)});
    }
    maxBoardTile /= 8;
    unsigned int numPossibleBonusTiles = int_log2(maxBoardTile) - 2;
    while (maxBoardTile >= 6) {
        debug(float(1)/numPossibleBonusTiles/21 > 1);
        result.push_back({maxBoardTile, float(1)/numPossibleBonusTiles/21});
        maxBoardTile /= 2;
    }
    return result;
}

ostream& outputTile(ostream &os, unsigned int tile) {
    if (tile != 0) {
        return os << setw(4) << tile;
    } else {
        return os << "    ";
    }
}

ostream& operator<<(ostream &os, BoardState const& board) {
    os << "Upcoming: " << board.nextTileHint() << endl;
    if (board.isGameOver()) {
        os << "Final";
    } else {
        os << "Current";
    }
    os << " Score: " << board.score() << endl;
    os << "---------------------" << endl;
    os << "|"; outputTile(os, board.board[0]) << "|"; outputTile(os, board.board[1]) << "|"; outputTile(os, board.board[2]) << "|"; outputTile(os, board.board[3]) << "|" << endl;
    os << "|"; outputTile(os, board.board[4]) << "|"; outputTile(os, board.board[5]) << "|"; outputTile(os, board.board[6]) << "|"; outputTile(os, board.board[7]) << "|" << endl;
    os << "|"; outputTile(os, board.board[8]) << "|"; outputTile(os, board.board[9]) << "|"; outputTile(os, board.board[10]) << "|"; outputTile(os, board.board[11]) << "|" << endl;
    os << "|"; outputTile(os, board.board[12]) << "|"; outputTile(os, board.board[13]) << "|"; outputTile(os, board.board[14]) << "|"; outputTile(os, board.board[15]) << "|" << endl;
    os << "---------------------";
    return os;
}

const BoardState BoardState::moveWithoutAdd(Direction d) const {
    BoardState copy = this->mutableCopy();
    copy.numTurns++;
    
    vector<pair<BoardIndex, BoardIndex>> movements;
    
    bool successfulMerge = false;
    bool countUp;
    bool countFirst;
    switch (d) {
        case UP:
            countUp = true;
            countFirst = false;
            break;
        case DOWN:
            countUp = false;
            countFirst = false;
            break;
        case LEFT:
            countUp = true;
            countFirst = true;
            break;
        case RIGHT:
            countUp = false;
            countFirst = true;
            break;
    }
    for (unsigned i = 0; i < 4; i++) {
        for (unsigned j = countUp ? 0 : 3; (countUp && j < 3) || (!countUp && j > 0); j += countUp? 1 : -1) {
            BoardIndex target = countFirst ? BoardIndex(j, i) : BoardIndex(i, j);
            BoardIndex here = countFirst ? BoardIndex(j + (countUp ? 1 : -1), i) : BoardIndex(i, j + (countUp ? 1 : -1));
            if (copy.canMerge(target, here)) {
                successfulMerge = true;
                copy.board[target.first + target.second*4] = copy.at(target) + copy.at(here);
                copy.board[here.first + here.second*4] = 0;
            }
        }
    }
    if (!successfulMerge) {
        throw InvalidMoveException();
    }
    return copy;
}

BoardState BoardState::addSpecificTile(Direction d, BoardIndex const& p, const unsigned int t) const {
    BoardState copy = this->addTile(d); //force RNG to advance the same number of times as if the tile had been added the natural way.
    copy.board = this->board;
    copy.board[p.first+p.second*4] = t;
    return copy;
}

const BoardState BoardState::addTile(Direction d) const {
    auto indices = this->validIndicesForNewTile(d);
    default_random_engine genCopy = this->generator;
    
    shuffle(indices.begin(), indices.end(), genCopy);
    unsigned int nextTileValue = this->upcomingTile();
    
    BoardState copy = this->mutableCopy();
    copy.board[indices.begin()->first + indices.begin()->second*4] = nextTileValue;
    switch (nextTileValue) {
        case 1:
            copy.onesInStack--;
            break;
        case 2:
            copy.twosInStack--;
            break;
        case 3:
            copy.threesInStack--;
            break;
        default:
            break;
    }
    if (copy.onesInStack + copy.twosInStack + copy.threesInStack == 0) {
        copy.onesInStack = 4;
        copy.twosInStack = 4;
        copy.threesInStack = 4;
    }
    copy.generator = genCopy;
    return copy;
}

const BoardState BoardState::move(Direction d) const {
    return this->moveWithoutAdd(d).addTile(d);
}

vector<Direction> BoardState::validMoves() const {
    vector<Direction> result;
    result.reserve(4);
    for (auto&& d : {DOWN, UP, LEFT, RIGHT}) {
        if (this->canMove(d)) {
            result.push_back(d);
        }
    }
    return result;
}

Direction BoardState::randomValidMove(default_random_engine generator) const {
    vector<Direction> moves = this->validMoves();
    shuffle(moves.begin(), moves.end(), generator);
    return moves[0];
}

BoardState BoardState::mutableCopy() const {
    BoardState result = *this;
    result.scoreCacheIsValid = false;
    result.isGameOverCacheIsValid = false;
    return result;
}

const BoardState BoardState::copyWithDifferentFuture() const {
    static seed_seq seeder({1,2,3});
    BoardState copy = *this;
    copy.generator = default_random_engine();
    copy.generator.seed(seeder);
    return copy;
}

unsigned int BoardState::maxTile() const {
    return *max_element(board.begin(), board.end());
}

unsigned int tileScore(unsigned int tileValue) {
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

unsigned int BoardState::score() const {
    if (this->scoreCacheIsValid) {
        return this->scoreCache;
    } else {
        this->scoreCacheIsValid = true;
        this->scoreCache = accumulate(this->board.begin(), this->board.end(), 0, [](unsigned int acc, unsigned int tile){
            return acc + tileScore(tile);
        });
        return this->scoreCache;
    }
}