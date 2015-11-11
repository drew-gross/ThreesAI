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
            Tile targetValue = this->at(target);
            Tile hereValue = this->at(here);
            if (canMerge(targetValue, hereValue)) {
                Tile newTargetValue = targetValue != Tile::EMPTY ? succ(hereValue) : hereValue;
                this->set(target, newTargetValue);
                if (this->maxTileCache < newTargetValue) {
                    this->maxTileCache = newTargetValue;
                }
                this->set(here, Tile::EMPTY);
                successfulMerge = true;
            }
        }
    }
    if (__builtin_expect(!successfulMerge, 0)) {
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
    for (auto&& i : allIndices) {
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
    this->generator = default_random_engine(d.howDifferent);
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
        array<Tile, 5> inRangeTiles;
        unsigned char tilesIndexEnd = 0;
        Tile hint1 = Tile::EMPTY;
        Tile hint2 = Tile::EMPTY;
        Tile hint3 = Tile::EMPTY;
        Tile actualTile = this->upcomingTile.value_or(this->genUpcomingTile());
        if (actualTile <= Tile::TILE_3) {
            return Hint(actualTile);
        } else {
            Tile maxBonusTile = this->maxBonusTile();
            //Add tiles that could show up
            if (pred(pred(actualTile)) >= Tile::TILE_6) {
                inRangeTiles[tilesIndexEnd] = pred(pred(actualTile));
                tilesIndexEnd++;
            }
            if (pred(actualTile) >= Tile::TILE_6) {
                inRangeTiles[tilesIndexEnd] = pred(actualTile);
                tilesIndexEnd++;
            }
            inRangeTiles[tilesIndexEnd] = actualTile;
            tilesIndexEnd++;
            if (succ(actualTile) <= maxBonusTile) {
                inRangeTiles[tilesIndexEnd] = succ(actualTile);
                tilesIndexEnd++;
            }
            if (succ(succ(actualTile)) <= maxBonusTile) {
                inRangeTiles[tilesIndexEnd] = succ(succ(actualTile));
                tilesIndexEnd++;
            }
            
            unsigned char tilesIndexBegin = 0;
            default_random_engine rngCopy = this->generator;
            //Trim list down to 3
            while (tilesIndexEnd - tilesIndexBegin > 3) {
                if (upcomingTile == inRangeTiles[tilesIndexEnd-1]) {
                    tilesIndexBegin++;
                } else if (upcomingTile == inRangeTiles[tilesIndexBegin]) {
                    tilesIndexEnd--;
                } else if (uniform_int_distribution<>(0,1)(rngCopy) == 1) {
                    tilesIndexEnd--;
                } else {
                    tilesIndexBegin++;
                }
            }
            
            if (tilesIndexEnd - tilesIndexBegin == 3) {
                hint3 = inRangeTiles[tilesIndexBegin + 2];
            }
            if (tilesIndexEnd - tilesIndexBegin >= 2) {
                hint2 = inRangeTiles[tilesIndexBegin + 1];
            }
            hint1 = inRangeTiles[tilesIndexBegin];
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
    return this->board[p.toRegularIndex()];
}

bool BoardState::isGameOver() const {
    return this->validMoves().size() == 0;
}

bool BoardState::canMove(Direction d) const {
    return this->validMoves().isEnabled(d);
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


static std::array<BoardIndex, 4> leftEdge = {BoardIndex(3,0),BoardIndex(3,1),BoardIndex(3,2),BoardIndex(3,3)};
static std::array<BoardIndex, 4> rightEdge = {BoardIndex(0,0),BoardIndex(0,1),BoardIndex(0,2),BoardIndex(0,3)};
static std::array<BoardIndex, 4> upEdge = {BoardIndex(0,3),BoardIndex(1,3),BoardIndex(2,3),BoardIndex(3,3)};
static std::array<BoardIndex, 4> downEdge = {BoardIndex(0,0),BoardIndex(1,0),BoardIndex(2,0),BoardIndex(3,0)};
EnabledIndices BoardState::validIndicesForNewTile(Direction movedDirection) const {
    std::array<BoardIndex, 4>* indices;
    switch (movedDirection) {
        case Direction::LEFT:
            indices = &leftEdge;
            break;
        case Direction::RIGHT:
            indices = &rightEdge;
            break;
        case Direction::UP:
            indices = &upEdge;
            break;
        case Direction::DOWN:
            indices = &downEdge;
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
    os << board.getHint() << endl;
    os << "---------------------" << endl;
    os << "|"; outputTile(os, board.board[0]) << "|"; outputTile(os, board.board[1]) << "|"; outputTile(os, board.board[2]) << "|"; outputTile(os, board.board[3]) << "| Score: " << board.score() << endl;
    os << "|"; outputTile(os, board.board[4]) << "|"; outputTile(os, board.board[5]) << "|"; outputTile(os, board.board[6]) << "|"; outputTile(os, board.board[7]) << "| Turns: " << board.numTurns << endl;
    os << "|"; outputTile(os, board.board[8]) << "|"; outputTile(os, board.board[9]) << "|"; outputTile(os, board.board[10]) << "|"; outputTile(os, board.board[11]) << "|" << endl;
    os << "|"; outputTile(os, board.board[12]) << "|"; outputTile(os, board.board[13]) << "|"; outputTile(os, board.board[14]) << "|"; outputTile(os, board.board[15]) << "|" << endl;
    os << "---------------------";
    return os;
}

EnabledDirections BoardState::validMoves() const {
    if (this->validMovesCacheIsValid) {
        return this->validMovesCache;
    }
    
    this->validMovesCache = EnabledDirections();
    for (unsigned i = 0; i < 4; i++) {
        if (canMerge(this->at(BoardIndex(i, 0)), this->at(BoardIndex(i, 1)))) {
            this->validMovesCache.set(Direction::UP);
            if (this->at(BoardIndex(i,0)) != Tile::EMPTY) {
                this->validMovesCache.set(Direction::DOWN);
            }
            break;
        }
        if (canMerge(this->at(BoardIndex(i, 1)), this->at(BoardIndex(i, 2)))) {
            this->validMovesCache.set(Direction::UP);
            if (this->at(BoardIndex(i,1)) != Tile::EMPTY) {
                this->validMovesCache.set(Direction::DOWN);
            }
            break;
        }
        if (canMerge(this->at(BoardIndex(i, 2)), this->at(BoardIndex(i, 3)))) {
            this->validMovesCache.set(Direction::UP);
            if (this->at(BoardIndex(i,2)) != Tile::EMPTY) {
                this->validMovesCache.set(Direction::DOWN);
            }
            break;
        }
    }
    if (!this->validMovesCache.isEnabled(Direction::DOWN)) {
        for (unsigned i = 0; i < 4; i++) {
            if (canMerge(this->at(BoardIndex(i, 3)), this->at(BoardIndex(i, 2))) ||
                canMerge(this->at(BoardIndex(i, 2)), this->at(BoardIndex(i, 1))) ||
                canMerge(this->at(BoardIndex(i, 1)), this->at(BoardIndex(i, 0)))) {
                this->validMovesCache.set(Direction::DOWN);
                break;
            }
        }
    }
    
    for (unsigned i = 0; i < 4; i++) {
        if (canMerge(this->at(BoardIndex(0, i)), this->at(BoardIndex(1, i)))) {
            this->validMovesCache.set(Direction::LEFT);
            if (this->at(BoardIndex(0, i)) != Tile::EMPTY) {
                this->validMovesCache.set(Direction::RIGHT);
            }
            break;
        }
        if (canMerge(this->at(BoardIndex(1, i)), this->at(BoardIndex(2, i)))) {
            this->validMovesCache.set(Direction::LEFT);
            if (this->at(BoardIndex(1, i)) != Tile::EMPTY) {
                this->validMovesCache.set(Direction::RIGHT);
            }
            break;
        }
        if (canMerge(this->at(BoardIndex(2, i)), this->at(BoardIndex(3, i)))) {
            this->validMovesCache.set(Direction::LEFT);
            if (this->at(BoardIndex(2, i)) != Tile::EMPTY) {
                this->validMovesCache.set(Direction::RIGHT);
            }
            break;
        }
    }
    if (!this->validMovesCache.isEnabled(Direction::RIGHT)) {
        for (unsigned i = 0; i < 4; i++) {
            if (canMerge(this->at(BoardIndex(3, i)), this->at(BoardIndex(2, i))) ||
                canMerge(this->at(BoardIndex(2, i)), this->at(BoardIndex(1, i))) ||
                canMerge(this->at(BoardIndex(1, i)), this->at(BoardIndex(0, i)))) {
                this->validMovesCache.set(Direction::RIGHT);
                break;
            }
        }
    }
    this->validMovesCacheIsValid = true;
    return this->validMovesCache;
}

Direction BoardState::randomValidMoveFromInternalGenerator() const {
    EnabledDirections moves = this->validMoves();
    default_random_engine genCopy = this->generator;
    int index = uniform_int_distribution<>(0,int(moves.size() - 1))(genCopy);
    for (Direction d : allDirections) {
        if (moves.isEnabled(d)) {
            if (index > 0) {
                index--;
            } else {
                return d;
            }
        }
    }
    debug();
    return Direction::LEFT;
}

BoardState::Score BoardState::runRandomSimulation(unsigned int simNumber) const {
    BoardState copy(BoardState::DifferentFuture(simNumber), *this);
    bool print = false;
    while (!copy.isGameOver()) {
        if (print) {
            MYLOG(copy);
        }
        try {
        copy.takeTurnInPlace(copy.randomValidMoveFromInternalGenerator());
        } catch (std::exception e) {
            debug();
            MYLOG(copy);
            copy.randomValidMoveFromInternalGenerator();
        }
    }
    return copy.score();
}

default_random_engine getGenerator() {
    static default_random_engine metaGenerator;
    return default_random_engine(metaGenerator());
}

Tile BoardState::maxTile() const {
    if (this->maxTileCache != Tile::EMPTY) {
        return this->maxTileCache;
    }
    this->maxTileCache = *max_element(board.begin(), board.end());
    return this->maxTileCache;
}

BoardState::Score BoardState::score() const {
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