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
                       Tile upcomingTile,
                       default_random_engine gen,
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
generator(gen),
upcomingTile(upcomingTile)
{
    this->hint = make_shared<RandomHint>(upcomingTile, this->maxBonusTile(), this->generator);
}

BoardState::BoardState(Board b,
                       std::default_random_engine gen,
                       std::shared_ptr<Hint const> hint,
                       unsigned int numTurns,
                       cv::Mat sourceImage,
                       unsigned int onesInStack,
                       unsigned int twosInStack,
                       unsigned int threesInStack) :
board(b),
hint(hint),
isGameOverCacheIsValid(false),
scoreCacheIsValid(false),
numTurns(numTurns),
sourceImage(sourceImage),
onesInStack(onesInStack),
twosInStack(twosInStack),
threesInStack(threesInStack),
generator(gen) {
    this->upcomingTile = hint->actualTile(gen);
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
generator(hintGen) {
    if (onesInStack + twosInStack + threesInStack == 0) {
        this->onesInStack = 4;
        this->twosInStack = 4;
        this->threesInStack = 4;
    }
    this->upcomingTile = this->genUpcomingTile();
    this->hint = make_shared<RandomHint>(upcomingTile, this->maxBonusTile(), this->generator);
}

Tile BoardState::genUpcomingTile() const {
    //This function contains an inlined version of possibleNextTiles, for performance reasons (to avoid creating a deque)
    
    // Due to floating point error, the sum of the probabilities for each tile may not add to 1,
    // which means if a 1 is generated for tileFinder, we get here. In this case, use 6,
    // which would have been returned had there been no floating point error.
    Tile maxBoardTile = this->maxTile();
    bool canHaveBonus = maxBoardTile >= Tile::TILE_48;
    default_random_engine genCopy = this->generator;
    uniform_real_distribution<> r(0,1);
    float tileFinder = r(genCopy);
    
    if (this->onesInStack > 0) {
        float pOne = this->nonBonusTileProbability(Tile::TILE_1, canHaveBonus);
        if (tileFinder < pOne) {
            return Tile::TILE_1;
        } else {
            tileFinder -= pOne;
        }
    }
    
    if (this->twosInStack > 0) {
        float pTwo = this->nonBonusTileProbability(Tile::TILE_2, canHaveBonus);
        if (tileFinder < pTwo) {
            return Tile::TILE_2;
        } else {
            tileFinder -= pTwo;
        }
    }
    
    if (this->threesInStack > 0) {
        float pThree = this->nonBonusTileProbability(Tile::TILE_3, canHaveBonus);
        if (tileFinder < pThree) {
            return Tile::TILE_3;
        } else {
            tileFinder -= pThree;
        }
    }
    
    Tile currentBonus = pred(pred(pred(maxBoardTile)));
    Tile possibleBonusCounter = currentBonus;
    unsigned int numPossibleBonusTiles = 0;
    while (possibleBonusCounter > Tile::TILE_3) {
        possibleBonusCounter = pred(possibleBonusCounter);
        numPossibleBonusTiles++;
    }
    while (currentBonus >= Tile::TILE_6) {
        float pThisBonus = float(1)/numPossibleBonusTiles/21;
        if (tileFinder < pThisBonus) {
            return currentBonus;
        } else {
            tileFinder -= pThisBonus;
            currentBonus = pred(currentBonus);
        }
    }
    return Tile::TILE_6;
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
    
    std::array<Tile, 16> tileList;
    transform(nums.begin(), nums.end(), tileList.begin(), [](string s){
        return tileFromString(s);
    });
    
    deque<Tile> hint(nextTileHintStrings.size());
    transform(nextTileHintStrings.begin(), nextTileHintStrings.end(), hint.begin(), [](string s) {
        return tileFromString(s);
    });
    
    //TODO: get numTurns and source image and values in stack from somewhere
    if (hint.size() == 1) {
        return BoardState(tileList, default_random_engine(0), make_shared<ForcedHint>(hint[0]), 0, cv::Mat(), 4, 4, 4);
    } else if (hint.size() == 2) {
        return BoardState(tileList, default_random_engine(0), make_shared<ForcedHint>(hint[0], hint[1]), 0, cv::Mat(), 4, 4, 4);
    } else {
        debug(hint.size() != 3);
        return BoardState(tileList, default_random_engine(0), make_shared<ForcedHint>(hint[0], hint[1], hint[2]), 0, cv::Mat(), 4, 4, 4);
    }
}

Tile BoardState::at(BoardIndex const& p) const {
    return this->board[p.first+p.second*4];
}

bool BoardState::isGameOver() const {
    if (!this->isGameOverCacheIsValid) {
        this->isGameOverCache = this->validMoves().empty();
        this->isGameOverCacheIsValid = true;
    }
    return this->isGameOverCache;
}

bool BoardState::canMove(Direction d) const {
    switch (d) {
        case Direction::UP:
            for (unsigned i = 0; i < 4; i++) {
                if (mergeResult(this->at({i, 0}), this->at({i, 1})) ||
                    mergeResult(this->at({i, 1}), this->at({i, 2})) ||
                    mergeResult(this->at({i, 2}), this->at({i, 3}))) {
                    return true;
                }
            }
            break;
        case Direction::DOWN:
            for (unsigned i = 0; i < 4; i++) {
                if (mergeResult(this->at({i, 3}), this->at({i, 2})) ||
                    mergeResult(this->at({i, 2}), this->at({i, 1})) ||
                    mergeResult(this->at({i, 1}), this->at({i, 0}))) {
                    return true;
                }
            }
            break;
        case Direction::LEFT:
            for (unsigned i = 0; i < 4; i++) {
                if (mergeResult(this->at({0, i}), this->at({1, i})) ||
                    mergeResult(this->at({1, i}), this->at({2, i})) ||
                    mergeResult(this->at({2, i}), this->at({3, i}))) {
                    return true;
                }
            }
            break;
        case Direction::RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                if (mergeResult(this->at({3, i}), this->at({2, i})) ||
                    mergeResult(this->at({2, i}), this->at({1, i})) ||
                    mergeResult(this->at({1, i}), this->at({0, i}))) {
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
    return pred(pred(pred(this->maxTile())));
}

bool BoardState::hasSameTilesAs(BoardState const& otherBoard, vector<BoardState::BoardIndex> excludedIndices) const {
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            BoardIndex curIndex(i,j);
            if (find(excludedIndices.begin(), excludedIndices.end(), curIndex) == excludedIndices.end()) {
                Tile tile = this->at(curIndex);
                Tile otherTile = otherBoard.at(curIndex);
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
        case Direction::LEFT:
            indicies = {BoardIndex(3,0),BoardIndex(3,1),BoardIndex(3,2),BoardIndex(3,3)};
            break;
        case Direction::RIGHT:
            indicies = {BoardIndex(0,0),BoardIndex(0,1),BoardIndex(0,2),BoardIndex(0,3)};
            break;
        case Direction::UP:
            indicies = {BoardIndex(0,3),BoardIndex(1,3),BoardIndex(2,3),BoardIndex(3,3)};
            break;
        case Direction::DOWN:
            indicies = {BoardIndex(0,0),BoardIndex(1,0),BoardIndex(2,0),BoardIndex(3,0)};
            break;
        default:
            break;
    }
    vector<BoardIndex> result;
    for (auto&& index : indicies) {
        if (this->at(index) == Tile::EMPTY) {
            result.push_back(index);
        }
    }
    return result;
}

unsigned int BoardState::stackSize() const {
    return this->onesInStack + this->twosInStack + this->threesInStack;
}

float BoardState::nonBonusTileProbability(Tile tile, bool canHaveBonus) const {
    unsigned int count = 0;
    switch (tile) {
        case Tile::TILE_1:
            count = this->onesInStack;
            break;
        case Tile::TILE_2:
            count = this->twosInStack;
            break;
        case Tile::TILE_3:
            count = this->threesInStack;
            break;
        default:
            debug();
    }
    float nonBonusProbability = float(count)/this->stackSize();
    if (canHaveBonus) {
        nonBonusProbability *= float(20)/21;
    }
    return nonBonusProbability;
}

deque<pair<Tile, float>> BoardState::possibleNextTiles() const {
    Tile maxBoardTile = this->maxTile();
    bool canHaveBonus = maxBoardTile >= Tile::TILE_48;
    deque<pair<Tile, float>> result;
    //should be able to only add 1,2,3 if they are in the stack
    if (this->onesInStack > 0) {
        result.push_back({Tile::TILE_1, this->nonBonusTileProbability(Tile::TILE_1, canHaveBonus)});
    }
    if (this->twosInStack > 0) {
        result.push_back({Tile::TILE_2, this->nonBonusTileProbability(Tile::TILE_2, canHaveBonus)});
    }
    if (this->threesInStack > 0) {
        result.push_back({Tile::TILE_3, this->nonBonusTileProbability(Tile::TILE_3, canHaveBonus)});
    }
    Tile possibleBonusTileCounter = pred(pred(pred(maxBoardTile)));
    unsigned int numPossibleBonusTiles = 0;
    while (possibleBonusTileCounter > Tile::TILE_3) {
        possibleBonusTileCounter = pred(possibleBonusTileCounter);
        numPossibleBonusTiles++;
    }
    
    Tile availableBonusTile = pred(pred(pred(maxBoardTile)));
    while (availableBonusTile > Tile::TILE_3) {
        result.push_back({availableBonusTile, float(1)/numPossibleBonusTiles/21});
        availableBonusTile = pred(availableBonusTile);
    }
    return result;
}

ostream& outputTile(ostream &os, Tile tile) {
    if (tile != Tile::EMPTY) {
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
    
    bool successfulMerge = false;
    bool countUp;
    bool countFirst;
    switch (d) {
        case Direction::UP:
            countUp = true;
            countFirst = false;
            break;
        case Direction::DOWN:
            countUp = false;
            countFirst = false;
            break;
        case Direction::LEFT:
            countUp = true;
            countFirst = true;
            break;
        case Direction::RIGHT:
            countUp = false;
            countFirst = true;
            break;
    }
    for (unsigned i = 0; i < 4; i++) {
        for (unsigned j = countUp ? 0 : 3; (countUp && j < 3) || (!countUp && j > 0); j += countUp? 1 : -1) {
            BoardIndex target = countFirst ? BoardIndex(j, i) : BoardIndex(i, j);
            BoardIndex here = countFirst ? BoardIndex(j + (countUp ? 1 : -1), i) : BoardIndex(i, j + (countUp ? 1 : -1));
            optional<Tile> merged = mergeResult(copy.at(target), copy.at(here));
            if (merged) {
                successfulMerge = true;
                copy.board[target.first + target.second*4] = merged.value();
                copy.board[here.first + here.second*4] = Tile::EMPTY;
            }
        }
    }
    if (!successfulMerge) {
        throw InvalidMoveException();
    }
    return copy;
}

BoardState BoardState::addSpecificTile(Direction d, BoardIndex const& p, const Tile t) const {
    BoardState copy = this->addTile(d); //force RNG to advance the same number of times as if the tile had been added the natural way.
    copy.board = this->board;
    copy.onesInStack = this->onesInStack;
    copy.twosInStack = this->twosInStack;
    copy.threesInStack = this->threesInStack;
    copy.board[p.first+p.second*4] = t;
    switch (t) {
        case Tile::TILE_1:
            copy.onesInStack--;
            break;
        case Tile::TILE_2:
            copy.twosInStack--;
            break;
        case Tile::TILE_3:
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
    
    BoardState copy(
        this->board,
        genCopy,
        this->numTurns + 1,
        this->sourceImage,
        this->onesInStack - (this->upcomingTile == Tile::TILE_1 ? 1 : 0),
        this->twosInStack - (this->upcomingTile == Tile::TILE_2 ? 1 : 0),
        this->threesInStack - (this->upcomingTile == Tile::TILE_3 ? 1 : 0)
    );
    
    copy.board[indices.begin()->first + indices.begin()->second*4] = this->upcomingTile;
    copy.generator = genCopy;
    return copy;
}

const BoardState BoardState::move(Direction d) const {
    return this->moveWithoutAdd(d).addTile(d);
}

vector<Direction> BoardState::validMoves() const {
    if (this->validMovesCacheIsValid) {
        return this->validMovesCache;
    }
    this->validMovesCache.clear();
    this->validMovesCache.reserve(4);
    for (auto&& d : {Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT}) {
        if (this->canMove(d)) {
            this->validMovesCache.push_back(d);
        }
    }
    return this->validMovesCache;
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
    result.validMovesCacheIsValid = false;
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

Tile BoardState::maxTile() const {
    return *max_element(board.begin(), board.end());
}

unsigned int BoardState::score() const {
    if (this->scoreCacheIsValid) {
        return this->scoreCache;
    } else {
        this->scoreCacheIsValid = true;
        this->scoreCache = accumulate(this->board.begin(), this->board.end(), 0, [](unsigned int acc, Tile tile){
            return acc + tileScore(tile);
        });
        return this->scoreCache;
    }
}