//
//  BoardState.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "BoardState.h"

#include "InvalidMoveException.h"
#include "RandomHint.hpp"
#include "ForcedHint.hpp"

#include <iostream>
#include <memory>
#include <boost/algorithm/string.hpp>

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace boost;

BoardState::BoardState(Board b,
                       std::shared_ptr<Hint const> hint,
                       unsigned int numTurns,
                       cv::Mat sourceImage,
                       unsigned int onesInStack,
                       unsigned int twosInStack,
                       unsigned int threesInStack) :
board(b),
isGameOverCacheIsValid(false),
scoreCacheIsValid(false),
numTurns(numTurns),
onesInStack(onesInStack),
twosInStack(twosInStack),
threesInStack(threesInStack),
sourceImage(sourceImage),
hint(hint)
{
    debug(!this->hint);
}

BoardState::BoardState(Board b,
                       default_random_engine hintGen,
                       unsigned int numTurns,
                       cv::Mat sourceImage,
                       unsigned int onesInStack,
                       unsigned int twosInStack,
                       unsigned int threesInStack) :
board(b),
isGameOverCacheIsValid(false),
scoreCacheIsValid(false),
numTurns(numTurns),
onesInStack(onesInStack),
twosInStack(twosInStack),
threesInStack(threesInStack),
sourceImage(sourceImage),
hint(new RandomHint(this->upcomingTile(), this->maxBonusTile(), this->generator)) {
    debug(!this->hint);
}

std::shared_ptr<Hint const> BoardState::getHint() const {
    return this->hint;
}

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
    if (hint.size() == 1) {
        return BoardState(tileList, make_shared<ForcedHint>(hint[0]), 0, cv::Mat(), 4, 4, 4);
    } else if (hint.size() == 2) {
        
        return BoardState(tileList, make_shared<ForcedHint>(hint[0], hint[1]), 0, cv::Mat(), 4, 4, 4);
    } else {
        debug(hint.size() != 3);
        return BoardState(tileList, make_shared<ForcedHint>(hint[0], hint[1], hint[2]), 0, cv::Mat(), 4, 4, 4);
    }
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

Tile BoardState::maxBonusTile() const {
    return this->maxTile()/8;
}

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
    Tile maxBoardTile = this->maxTile();
    bool canHaveBonus = maxBoardTile >= 48;
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
        result.push_back({maxBoardTile, float(1)/numPossibleBonusTiles/21});
        maxBoardTile /= 2;
    }
    return result;
}

Tile BoardState::upcomingTile() const {
    //This function contains an inlined version of possibleNextTiles, for performance reasons (to avoid creating a deque)
    Tile maxBoardTile = this->maxTile();
    bool canHaveBonus = maxBoardTile >= 48;
    default_random_engine genCopy = this->generator;
    uniform_real_distribution<> r(0,1);
    float tileFinder = r(genCopy);
        
    if (this->onesInStack > 0) {
        float pOne = this->nonBonusTileProbability(1, canHaveBonus);
        if (tileFinder < pOne) {
            return 1;
        } else {
            tileFinder -= pOne;
        }
    }
    
    if (this->twosInStack > 0) {
        float pTwo = this->nonBonusTileProbability(2, canHaveBonus);
        if (tileFinder < pTwo) {
            return 2;
        } else {
            tileFinder -= pTwo;
        }
    }
    
    if (this->threesInStack > 0) {
        float pThree = this->nonBonusTileProbability(3, canHaveBonus);
        if (tileFinder < pThree) {
            return 3;
        } else {
            tileFinder -= pThree;
        }
    }
    
    Tile currentBonus = maxBoardTile / 8;
    unsigned int numPossibleBonusTiles = int_log2(currentBonus) - 2;
    while (currentBonus >= 6) {
        float pThisBonus = float(1)/numPossibleBonusTiles/21;
        if (tileFinder < pThisBonus) {
            return currentBonus;
        } else {
            tileFinder -= pThisBonus;
            currentBonus /=2;
        }
    }
    
    // Due to floating point error, the sum of the probabilities for each tile may not add to 1,
    // which means if a 1 is generated for tileFinder, we get here. In this case, return 6,
    // which would have been returned had there been no floating point error.
    return 6;
}

ostream& outputTile(ostream &os, unsigned int tile) {
    if (tile != 0) {
        return os << setw(4) << tile;
    } else {
        return os << "    ";
    }
}

ostream& operator<<(ostream &os, BoardState const& board) {
    os << "Upcoming: " << *board.getHint() << endl;
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
    copy.onesInStack = this->onesInStack;
    copy.twosInStack = this->twosInStack;
    copy.threesInStack = this->threesInStack;
    copy.board[p.first+p.second*4] = t;
    switch (t) {
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
    return copy;
}

const BoardState BoardState::addTile(Direction d) const {
    auto indices = this->validIndicesForNewTile(d);
    default_random_engine genCopy = this->generator;
    
    shuffle(indices.begin(), indices.end(), genCopy);
    genCopy.discard(1); // If there is only one valid move, then the shuffle won't modify the generator, and the state will get stuck
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

Direction BoardState::randomValidMoveFromInternalGenerator() const {
    vector<Direction> moves = this->validMoves();
    default_random_engine genCopy = this->generator;
    shuffle(moves.begin(), moves.end(), genCopy);
    return moves[0];
}

BoardState BoardState::mutableCopy() const {
    BoardState result = *this;
    result.scoreCacheIsValid = false;
    result.isGameOverCacheIsValid = false;
    return result;
}

default_random_engine getGenerator() {
    static default_random_engine metaGenerator;
    return default_random_engine(metaGenerator());
}

const BoardState BoardState::copyWithDifferentFuture() const {
    BoardState copy = *this;
    copy.generator = getGenerator();
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