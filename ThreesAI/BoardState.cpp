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
#include <memory>
#include <boost/algorithm/string.hpp>

#include "Debug.h"
#include "Logging.h"

using namespace std;
using namespace boost;

void BoardState::set(BoardIndex i, Tile t) {
    this->board[i.toRegularIndex()] = t;
}

void BoardState::takeTurnInPlace(Direction d) {
    this->move(d);
    this->addTile(d);
    this->isGameOverCacheIsValid = false;
    this->scoreCacheIsValid = false;
    this->validMovesCacheIsValid = false;
}

void BoardState::move(Direction d) {
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
            optional<Tile> merged = mergeResult(this->at(target), this->at(here));
            if (merged) {
                successfulMerge = true;
                this->set(target, merged.value());
                this->set(here, Tile::EMPTY);
            }
        }
    }
    if (!successfulMerge) {
        throw InvalidMoveException();
    }
}

BoardState::BoardState(BoardState::MoveWithoutAdd m, BoardState const& other) {
    this->copy(other);
    this->move(m.d);
}

BoardState::BoardState(BoardState::AddSpecificTile t, BoardState const& other) {
    this->copy(other);
    this->indexForNextTile(t.d); //force RNG to advance the same number of times as if the tile had been added the natural way.
    this->upcomingTile = none;
    this->set(t.i, t.t);
    this->removeFromStack(t.t);
}

void BoardState::removeFromStack(Tile t) {
    switch (t) {
        case Tile::TILE_1:
            this->onesInStack--;
            break;
        case Tile::TILE_2:
            this->twosInStack--;
            break;
        case Tile::TILE_3:
            this->threesInStack--;
            break;
        default:
            break;
    }
    if (this->stackSize() == 0) {
        this->onesInStack = 4;
        this->twosInStack = 4;
        this->threesInStack = 4;
    }
}

BoardIndex BoardState::indexForNextTile(Direction d) {
    EnabledIndices validIndexes = this->validIndicesForNewTile(d);
    debug(validIndexes.size() == 0);
    uniform_int_distribution<unsigned long> dist(0,validIndexes.size() - 1);
    unsigned long indexWithinValid = dist(this->generator);
    for (BoardIndex i : allIndices) {
        if (validIndexes.isEnabled(i)) {
            if (indexWithinValid > 0) {
                indexWithinValid--;
            } else {
                return i;
            }
        }
    }
    debug();
    return BoardIndex(0,0);
}

__attribute__((noinline))
void BoardState::addTile(Direction d) {
    Tile upcomingTile = this->getUpcomingTile();
    BoardIndex i = this->indexForNextTile(d);
    this->numTurns++;
    this->removeFromStack(upcomingTile);
    this->set(i, upcomingTile);
    this->upcomingTile = none;
}

BoardState::BoardState(BoardState::AddTile t, BoardState const& other) {
    this->copy(other);
    this->addTile(t.d);
}

BoardState::BoardState(BoardState::Move m, BoardState const& other) {
    this->copy(other);
    this->takeTurnInPlace(m.d);
}

void BoardState::copy(BoardState const &other) {
    this->numTurns = other.numTurns;
    this->sourceImage = other.sourceImage;
    this->generator = other.generator;
    this->onesInStack = other.onesInStack;
    this->twosInStack = other.twosInStack;
    this->threesInStack = other.threesInStack;
    this->board = other.board;
    this->upcomingTile = other.upcomingTile;
    this->hint = other.hint;
}

BoardState::BoardState(BoardState::DifferentFuture d, BoardState const& other) {
    this->copy(other);
    this->generator.discard(d.howDifferent);
}

BoardState::BoardState(Board b,
                       std::default_random_engine gen,
                       Hint hint,
                       unsigned int numTurns,
                       cv::Mat sourceImage,
                       unsigned int onesInStack,
                       unsigned int twosInStack,
                       unsigned int threesInStack) :
board(b),
hint(hint),
numTurns(numTurns),
sourceImage(sourceImage),
onesInStack(onesInStack),
twosInStack(twosInStack),
threesInStack(threesInStack),
generator(gen) {
    if (this->stackSize() == 0) {
        this->onesInStack = 4;
        this->twosInStack = 4;
        this->threesInStack = 4;
    }
}

BoardState::BoardState(Board b,
                       default_random_engine hintGen,
                       unsigned int numTurns,
                       cv::Mat sourceImage,
                       unsigned int onesInStack,
                       unsigned int twosInStack,
                       unsigned int threesInStack) :
board(b),
numTurns(numTurns),
onesInStack(onesInStack),
twosInStack(twosInStack),
threesInStack(threesInStack),
sourceImage(sourceImage),
generator(hintGen),
hint(none) {
    if (this->stackSize() == 0) {
        this->onesInStack = 4;
        this->twosInStack = 4;
        this->threesInStack = 4;
    }
}


BoardState::BoardState(FromString s) :
generator(0),
onesInStack(4),
twosInStack(4),
threesInStack(4)
{
    vector<string> splitName;
    split(splitName, s.s, is_any_of("-"));
    
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
    
    if (hint.size() == 1) {
        this->board = tileList;
        this->hint = Hint(hint[0]);
    } else if (hint.size() == 2) {
        this->board = tileList;
        this->hint = Hint(hint[0], hint[1]);
    } else {
        debug(hint.size() != 3);
        this->board = tileList;
        this->hint = Hint(hint[0], hint[1], hint[2]);
    }
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

Hint BoardState::getHint() const {
    if (this->hint) {
        return this->hint.value();
    } else {
        deque<Tile> inRangeTiles;
        Tile hint1 = Tile::EMPTY;
        Tile hint2 = Tile::EMPTY;
        Tile hint3 = Tile::EMPTY;
        Tile actualTile = this->upcomingTile.value_or(this->genUpcomingTile());
        if (actualTile <= Tile::TILE_3) {
            return Hint(actualTile);
        } else {
            //Add tiles that could show up
            if (pred(pred(actualTile)) >= Tile::TILE_6) {
                inRangeTiles.push_back(pred(pred(actualTile)));
            }
            if (pred(actualTile) >= Tile::TILE_6) {
                inRangeTiles.push_back(pred(actualTile));
            }
            inRangeTiles.push_back(actualTile);
            if (succ(actualTile) <= this->maxBonusTile()) {
                inRangeTiles.push_back(succ(actualTile));
            }
            if (succ(succ(actualTile)) <= this->maxBonusTile()) {
                inRangeTiles.push_back(succ(succ(actualTile)));
            }
            
            default_random_engine rngCopy = this->generator;
            //Trim list down to 3
            while (inRangeTiles.size() > 3) {
                if (upcomingTile == *inRangeTiles.end()) {
                    inRangeTiles.pop_front();
                } else if (upcomingTile == *inRangeTiles.begin()) {
                    inRangeTiles.pop_back();
                } else if (uniform_int_distribution<>(0,1)(rngCopy) == 1) {
                    inRangeTiles.pop_back();
                } else {
                    inRangeTiles.pop_front();
                }
            }
            
            if (inRangeTiles.size() == 3) {
                hint3 = inRangeTiles[2];
            }
            if (inRangeTiles.size() >= 2) {
                hint2 = inRangeTiles[1];
            }
            hint1 = inRangeTiles[0];
        }
        
        Hint result(hint1, hint2, hint3);
        debug(!result.contains(actualTile));
        debug(hint1 > Tile::TILE_6144);
        debug(hint2 > Tile::TILE_6144);
        debug(hint3 > Tile::TILE_6144);
        
        return result;
    }
}

Tile BoardState::getUpcomingTile() {
    if (this->upcomingTile) {
        return this->upcomingTile.value();
    } else {
        return this->getHint().actualTile(this->generator);
    }
}

ostream& operator<<(ostream &os, const BoardIndex i){
    os << "{" << i.first << ", " << i.second << "}";
    return os;
}

Tile BoardState::at(BoardIndex const& p) const {
    return this->board[p.first+p.second*4];
}

bool BoardState::isGameOver() const {
    if (!this->isGameOverCacheIsValid) {
        if (this->validMovesCacheIsValid) {
            return this->validMovesCache.empty();
        }
        for (auto&& d : directions) {
            if (this->canMove(d)) {
                this->isGameOverCacheIsValid = true;
                this->isGameOverCache = false;
                return this->isGameOverCache;
            }
        }
        this->isGameOverCache = true;
        this->isGameOverCacheIsValid = true;
    }
    return this->isGameOverCache;
}

bool BoardState::canMove(Direction d) const {
    switch (d) {
        case Direction::UP:
            for (unsigned i = 0; i < 4; i++) {
                if (canMerge(this->at(BoardIndex(i, 0)), this->at(BoardIndex(i, 1))) ||
                    canMerge(this->at(BoardIndex(i, 1)), this->at(BoardIndex(i, 2))) ||
                    canMerge(this->at(BoardIndex(i, 2)), this->at(BoardIndex(i, 3)))) {
                    return true;
                }
            }
            break;
        case Direction::DOWN:
            for (unsigned i = 0; i < 4; i++) {
                if (canMerge(this->at(BoardIndex(i, 3)), this->at(BoardIndex(i, 2))) ||
                    canMerge(this->at(BoardIndex(i, 2)), this->at(BoardIndex(i, 1))) ||
                    canMerge(this->at(BoardIndex(i, 1)), this->at(BoardIndex(i, 0)))) {
                    return true;
                }
            }
            break;
        case Direction::LEFT:
            for (unsigned i = 0; i < 4; i++) {
                if (canMerge(this->at(BoardIndex(0, i)), this->at(BoardIndex(1, i))) ||
                    canMerge(this->at(BoardIndex(1, i)), this->at(BoardIndex(2, i))) ||
                    canMerge(this->at(BoardIndex(2, i)), this->at(BoardIndex(3, i)))) {
                    return true;
                }
            }
            break;
        case Direction::RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                if (canMerge(this->at(BoardIndex(3, i)), this->at(BoardIndex(2, i))) ||
                    canMerge(this->at(BoardIndex(2, i)), this->at(BoardIndex(1, i))) ||
                    canMerge(this->at(BoardIndex(1, i)), this->at(BoardIndex(0, i)))) {
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

bool BoardState::hasSameTilesAs(BoardState const& otherBoard, EnabledIndices excludedIndices) const {
    for (BoardIndex i : allIndices) {
        if (!excludedIndices.isEnabled(i)) {
            Tile tile = this->at(i);
            Tile otherTile = otherBoard.at(i);
            if (tile != otherTile) {
                return false;
            }
        }
    }
    return true;
}

EnabledIndices BoardState::validIndicesForNewTile(Direction movedDirection) const {
    static std::array<BoardIndex, 4> left = {BoardIndex(3,0),BoardIndex(3,1),BoardIndex(3,2),BoardIndex(3,3)};
    static std::array<BoardIndex, 4> right = {BoardIndex(0,0),BoardIndex(0,1),BoardIndex(0,2),BoardIndex(0,3)};
    static std::array<BoardIndex, 4> up = {BoardIndex(0,3),BoardIndex(1,3),BoardIndex(2,3),BoardIndex(3,3)};
    static std::array<BoardIndex, 4> down = {BoardIndex(0,0),BoardIndex(1,0),BoardIndex(2,0),BoardIndex(3,0)};
    
    std::array<BoardIndex, 4>* indices;
    switch (movedDirection) {
        case Direction::LEFT:
            indices = &left;
            break;
        case Direction::RIGHT:
            indices = &right;
            break;
        case Direction::UP:
            indices = &up;
            break;
        case Direction::DOWN:
            indices = &down;
            break;
        default:
            break;
    }
    EnabledIndices result({});
    for (auto&& index : *indices) {
        if (this->at(index) == Tile::EMPTY) {
            result.set(index);
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
    os << "Upcoming: " << board.getHint() << endl;
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
    this->validMovesCacheIsValid = true;
    return this->validMovesCache;
}

Direction BoardState::randomValidMoveFromInternalGenerator() const {
    vector<Direction> moves = this->validMoves();
    default_random_engine genCopy = this->generator;
    shuffle(moves.begin(), moves.end(), genCopy);
    return moves[0];
}

default_random_engine getGenerator() {
    static default_random_engine metaGenerator;
    return default_random_engine(metaGenerator());
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