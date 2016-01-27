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

SearchResult BoardState::heuristicSearchIfMovedInDirection(Direction d, uint8_t depth, Heuristic h) const {
    //Assume board was moved but hasn't had tile added
    auto allAdditions = this->possibleAdditions(d);
    float score = 0;
    unsigned int openNodeCount = 0;
    for (auto&& info : allAdditions) {
        BoardState potentialBoard(BoardState::AddSpecificTile(d, info.i, info.t), *this, true);
        if (depth == 0) {
            score += h(potentialBoard)*info.probability;
            openNodeCount += 1;
        } else {
            vector<pair<Direction, SearchResult>> scoresForMoves;
            for (auto&& d : allDirections) {
                if (potentialBoard.isMoveValid(d)) {
                    BoardState movedBoard(BoardState::MoveWithoutAdd(d), potentialBoard);
                    scoresForMoves.push_back({d, movedBoard.heuristicSearchIfMovedInDirection(d, depth - 1, h)});
                }
            }
            if (scoresForMoves.empty()) {
                //No need to bump open node count here, this node is not open. Also don't bump score, we want to assume death is to be avoided at all costs.
            } else {
                openNodeCount += accumulate(scoresForMoves.begin(), scoresForMoves.end(), 0, [](unsigned int soFar, pair<Direction, SearchResult> thisMove){
                    return soFar + thisMove.second.openNodes;
                });
                score += max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, SearchResult> left, pair<Direction, SearchResult> right){
                    return left.second.value < right.second.value;
                })->second.value;
            }
        }
    }
    return {score, openNodeCount};
}

bool HiddenBoardState::operator==(HiddenBoardState const& other) const {
    return this->numTurns == other.numTurns &&
        this->onesInStack == other.onesInStack &&
        this->twosInStack == other.twosInStack &&
        this->threesInStack == other.threesInStack;
}

void BoardState::set(BoardIndex i, Tile t) {
    this->board[i.toRegularIndex()] = t;
    this->maxTileCache = std::max(t, this->maxTileCache);
}

void BoardState::takeTurnInPlace(Direction d) {
    this->upcomingTile = this->getUpcomingTile(); //Must get upcoming tile before moving, as moveing can change which tile is added for a given generator state.
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
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = countUp ? 0 : 3; (countUp && j < 3) || (!countUp && j > 0); j += countUp? 1 : -1) {
            BoardIndex target = countFirst ? BoardIndex(j, i) : BoardIndex(i, j);
            BoardIndex here = countFirst ? BoardIndex(j + (countUp ? 1 : -1), i) : BoardIndex(i, j + (countUp ? 1 : -1));
            Tile targetValue = this->at(target);
            Tile hereValue = this->at(here);
            if (canMerge(targetValue, hereValue)) {
                Tile newTargetValue = targetValue != Tile::EMPTY ? succ(hereValue) : hereValue;
                this->set(target, newTargetValue);
                this->set(here, Tile::EMPTY);
                successfulMerge = true;
            }
        }
    }
    if (__builtin_expect(!successfulMerge, 0)) {
        throw InvalidMoveException();
    }
}

BoardState::BoardState(BoardState::MoveWithoutAdd m, BoardState const& other) : hiddenState(other.hiddenState) {
    this->copy(other);
    this->move(m.d);
}

BoardState::BoardState(BoardState::AddSpecificTile t, BoardState const& other, bool hasNoHint) : hiddenState(other.nextHiddenState(t.t)) {
    this->copy(other);
    this->indexForNextTile(t.d); //force RNG to advance the same number of times as if the tile had been added the natural way.
    this->upcomingTile = none;
    this->hint = none;
    this->set(t.i, t.t);
    this->hasNoHint = true;
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
    Tile t = this->getUpcomingTile(); //Note: getUpcomingTile changes the generator, so this must be retrieved before the index.
    BoardIndex i = this->indexForNextTile(d);
    this->set(i, t);
    this->hiddenState = this->nextHiddenState(t);
    this->upcomingTile = none;
    this->hint = none;
}

BoardState::BoardState(BoardState::AddTile t, BoardState const& other) : hiddenState(other.hiddenState) {
    this->copy(other);
    this->addTile(t.d);
}

BoardState::BoardState(BoardState::MoveWithAdd m, BoardState const& other) : hiddenState(other.hiddenState) {
    this->copy(other);
    this->takeTurnInPlace(m.d);
}

void BoardState::copy(BoardState const &other) {
    this->sourceImage = other.sourceImage;
    this->generator = other.generator;
    this->board = other.board;
    this->upcomingTile = other.upcomingTile;
    this->hint = other.hint;
    this->hasNoHint = other.hasNoHint;
    this->maxTileCache = other.maxTileCache;
}

BoardState::BoardState(BoardState::DifferentFuture d, BoardState const& other) : hiddenState(other.hiddenState) {
    this->copy(other);
    this->generator = default_random_engine(d.howDifferent);
}

BoardState::BoardState(Board b,
                       HiddenBoardState h,
                       std::default_random_engine gen,
                       Hint hint,
                       cv::Mat sourceImage) :
board(b),
hint(hint),
sourceImage(sourceImage),
hasNoHint(false),
generator(gen),
hiddenState(h) {}

BoardState::BoardState(Board b,
                       HiddenBoardState h,
                       default_random_engine hintGen,
                       cv::Mat sourceImage) :
board(b),
sourceImage(sourceImage),
generator(hintGen),
hasNoHint(false),
hint(none),
hiddenState(h) {}


BoardState::BoardState(FromString s) :
generator(0),
hiddenState(0,4,4,4),
hasNoHint(false)
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
    this->maxTileCache = *max_element(tileList.begin(), tileList.end());
}

Tile BoardState::genUpcomingTile() const {
    //This function contains an inlined version of possibleNextTiles, for performance reasons (to avoid creating a deque)
    
    // Due to floating point error, the sum of the probabilities for each tile may not add to 1,
    // which means if a 1 is generated for tileFinder, we get here. In this case, use 6,
    // which would have been returned had there been no floating point error.
    debug(this->hint != none);
    Tile maxBoardTile = this->maxTile();
    bool canHaveBonus = maxBoardTile >= Tile::TILE_48;
    default_random_engine genCopy = this->generator;
    uniform_real_distribution<> r(0,1);
    float tileFinder = r(genCopy);
    
    if (this->hiddenState.onesInStack > 0) {
        float pOne = this->nonBonusTileProbability(Tile::TILE_1, canHaveBonus);
        if (tileFinder < pOne) {
            return Tile::TILE_1;
        } else {
            tileFinder -= pOne;
        }
    }
    
    if (this->hiddenState.twosInStack > 0) {
        float pTwo = this->nonBonusTileProbability(Tile::TILE_2, canHaveBonus);
        if (tileFinder < pTwo) {
            return Tile::TILE_2;
        } else {
            tileFinder -= pTwo;
        }
    }
    
    if (this->hiddenState.threesInStack > 0) {
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

BoardState::BoardState(SetHint h, BoardState const& other) : hiddenState(other.hiddenState) {
    this->copy(other);
    debug(!this->hasNoHint);
    this->hasNoHint = false;
    this->hint = make_optional(h.h);
}

BoardState::BoardState(SetHiddenState h, BoardState const& other) : hiddenState(h.h) {
    this->copy(other);
}

long BoardState::countOfTile(Tile t) const {
    return count(this->board.begin(), this->board.end(), t);
}

unsigned long BoardState::adjacentPairCount() const {
    unsigned long count = 0;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Tile here = this->at(BoardIndex(i, j));
            if (i < 3) {
                Tile below = this->at(BoardIndex(i + 1, j));
                if (canMerge(here, below)) {
                    count++;
                }
            }
            if (j < 3) {
                Tile right = this->at(BoardIndex(i, j + 1));
                if (canMerge(here, right)) {
                    count++;
                }
            }
        }
    }
    return count;
}

unsigned long BoardState::adjacentOffByOneCount() const {
    unsigned long count = 0;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Tile here = this->at(BoardIndex(i, j));
            if (i < 3) {
                Tile below = this->at(BoardIndex(i + 1, j));
                if (canMerge(here, succ(below)) || canMerge(here, pred(below))) {
                    count++;
                }
            }
            if (j < 3) {
                Tile right = this->at(BoardIndex(i, j + 1));
                if (canMerge(here, succ(right)) || canMerge(here, pred(right))) {
                    count++;
                }
            }
        }
    }
    return count;
}

unsigned long BoardState::trappedTileCount() const {
    unsigned long count = 0;
    for (BoardIndex i : allIndices) {
        if (i.left() && i.right()) {
            if (this->at(i) < this->at(i.left().get()) &&
                this->at(i) < this->at(i.right().get())) {
                count++;
            }
        } else if (i.left() && this->at(i) < this->at(i.left().get())) {
            count++;
        } else if (i.right() && this->at(i) < this->at(i.right().get())) {
            count++;
        }
        
        if (i.up() && i.down()) {
            if (this->at(i) < this->at(i.up().get()) &&
                this->at(i) < this->at(i.down().get())) {
                count++;
            }
        } else if (i.up() && this->at(i) < this->at(i.up().get())) {
            count++;
        } else if (i.down() && this->at(i) < this->at(i.down().get())) {
            count++;
        }
    }
    return count;
}

unsigned long BoardState::splitPairCount() const {
    unsigned long count = 0;
    for (Tile t = Tile::TILE_3; t < Tile::TILE_6144; t = succ(t)) {
        for (unsigned char i = 0; i < 4; i++) {
            for (unsigned char j = 0; j < 4; j++) {
                Tile here = this->at(BoardIndex(i, j));
                bool hasAdjacent = false;
                if (i < 3) {
                    Tile below = this->at(BoardIndex(i + 1, j));
                    if (canMerge(here, below)) {
                        hasAdjacent = true;
                    }
                }
                if (j < 3) {
                    Tile right = this->at(BoardIndex(i, j + 1));
                    if (canMerge(here, right)) {
                        hasAdjacent = true;
                    }
                }
                if (!hasAdjacent && this->countOfTile(t) > 1) {
                    count++;
                }
            }
        }
    }
    return count;
}

Hint BoardState::getHint() const {
    debug(this->hasNoHint);
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

Tile BoardState::getUpcomingTile() const {
    if (this->upcomingTile) {
        return this->upcomingTile.value();
    } else {
        return this->getHint().actualTile(this->generator);
    }
}

ostream& operator<<(ostream &os, const BoardIndex i){
    int first = i.first;
    int second = i.second;
    os << "{" << first << ", " << second << "}";
    return os;
}

Tile BoardState::at(BoardIndex const& p) const {
    return this->board[p.toRegularIndex()];
}

bool BoardState::isGameOver() const {
    return this->validMoves().empty();
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

HiddenBoardState BoardState::nextHiddenState(boost::optional<Tile> mostRecentlyAddedTile) const {
    unsigned int newOnes = this->hiddenState.onesInStack;
    unsigned int newTwos = this->hiddenState.twosInStack;
    unsigned int newThrees = this->hiddenState.threesInStack;
    
    switch (mostRecentlyAddedTile.value_or_eval([this](){return this->getUpcomingTile();})) {
        case Tile::TILE_1:
            newOnes--;
            break;
        case Tile::TILE_2:
            newTwos--;
            break;
        case Tile::TILE_3:
            newThrees--;
            break;
        default:
            break;
    }
    return HiddenBoardState(this->hiddenState.numTurns + 1, newOnes, newTwos, newThrees);
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

float BoardState::nonBonusTileProbability(Tile tile, bool canHaveBonus) const {
    unsigned int count = 0;
    switch (tile) {
        case Tile::TILE_1:
            count = this->hiddenState.onesInStack;
            break;
        case Tile::TILE_2:
            count = this->hiddenState.twosInStack;
            break;
        case Tile::TILE_3:
            count = this->hiddenState.threesInStack;
            break;
        default:
            debug();
    }
    float nonBonusProbability = float(count)/this->hiddenState.stackSize();
    if (canHaveBonus) {
        nonBonusProbability *= float(20)/21;
    }
    return nonBonusProbability;
}

deque<pair<Tile, float>> BoardState::possibleNextTiles() const {
    debug(this->hint == none && !this->hasNoHint);
    Tile maxBoardTile = this->maxTile();
    bool canHaveBonus = maxBoardTile >= Tile::TILE_48;
    deque<pair<Tile, float>> result;
    //should be able to only add 1,2,3 if they are in the stack
    if (this->hiddenState.onesInStack > 0) {
        result.push_back({Tile::TILE_1, this->nonBonusTileProbability(Tile::TILE_1, canHaveBonus)});
    }
    if (this->hiddenState.twosInStack > 0) {
        result.push_back({Tile::TILE_2, this->nonBonusTileProbability(Tile::TILE_2, canHaveBonus)});
    }
    if (this->hiddenState.threesInStack > 0) {
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

deque<BoardState::AdditionInfo> BoardState::possibleAdditions(Direction directionMovedToGetHere) const {
    deque<BoardState::AdditionInfo> results;
    
    deque<pair<Tile, float>> possibleNextTiles;
    if (this->hasNoHint) {
        possibleNextTiles = this->possibleNextTiles();
    } else {
        possibleNextTiles = this->getHint().possibleTiles();
    }
    EnabledIndices possibleNextLocations = this->validIndicesForNewTile(directionMovedToGetHere);
    
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (BoardIndex i : allIndices) {
            if (possibleNextLocations.isEnabled(i)) {
                results.push_back({nextTile.first, i, nextTile.second*locationProbability});
            }
        }
    }
    return results;
}

ostream& operator<<(ostream &os, BoardState const& board) {
    if (board.hasNoHint) {
        os << "No Hint" << endl;
    } else {
        os << "Hint: " << board.getHint() << endl;
    }
    os << "Turns: " << board.hiddenState.numTurns << endl;
    os << "Score: " << board.score() << endl;
    os << "Stack: " << board.hiddenState.onesInStack << " " << board.hiddenState.twosInStack << " " << board.hiddenState.threesInStack << endl;
    os << "---------------------" << endl;
    os << "|" << board.board[0] << "|" << board.board[1] << "|" << board.board[2] << "|" << board.board[3] << "|" << endl;
    os << "|" << board.board[4] << "|" << board.board[5] << "|" << board.board[6] << "|" << board.board[7] << "|" << endl;
    os << "|" << board.board[8] << "|" << board.board[9] << "|" << board.board[10] << "|" << board.board[11] << "|" << endl;
    os << "|" << board.board[12] << "|" << board.board[13] << "|" << board.board[14] << "|" << board.board[15] << "|" << endl;
    os << "---------------------";
    return os;
}

bool BoardState::isMoveValid(Direction d) const {
    return this->validMoves().isEnabled(d);
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
    copy.hasNoHint = false;
    while (!copy.isGameOver()) {
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