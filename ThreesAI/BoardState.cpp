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

#include "Heuristic.hpp"

using namespace std;
using namespace boost;

float nonBonusTileProbability(HiddenBoardState const& hiddenState, Tile tile, bool canHaveBonus) {
    unsigned int count = 0;
    switch (tile.value) {
        case T::_1:
            count = hiddenState.onesInStack;
            break;
        case T::_2:
            count = hiddenState.twosInStack;
            break;
        case T::_3:
            count = hiddenState.threesInStack;
            break;
        default:
            debug();
    }
    float nonBonusProbability = float(count)/hiddenState.stackSize();
    if (canHaveBonus) {
        nonBonusProbability *= float(20)/21;
    }
    return nonBonusProbability;
}

Tile UpcomingTileGenerator::generateTile(std::default_random_engine rng, Tile maxBoardTile, HiddenBoardState hiddenState) const {
    if (this->upcomingTile) return this->upcomingTile.get();
    if (this->hint) {
        return this->hint.get().actualTile(rng);
    };
    
    bool canHaveBonus = maxBoardTile >= T::_48;
    
    std::uniform_real_distribution<> r(0,1);
    float tileFinder = r(rng);
    
    if (hiddenState.onesInStack > 0) {
        float pOne = nonBonusTileProbability(hiddenState, T::_1, canHaveBonus);
        if (tileFinder < pOne) {
            return T::_1;
        } else {
            tileFinder -= pOne;
        }
    }
    
    if (hiddenState.twosInStack > 0) {
        float pTwo = nonBonusTileProbability(hiddenState,T::_2, canHaveBonus);
        if (tileFinder < pTwo) {
            return T::_2;
        } else {
            tileFinder -= pTwo;
        }
    }
    
    if (hiddenState.threesInStack > 0) {
        float pThree = nonBonusTileProbability(hiddenState,T::_3, canHaveBonus);
        if (tileFinder < pThree) {
            return T::_3;
        } else {
            tileFinder -= pThree;
        }
    }
    
    Tile currentBonus = maxBoardTile.pred().pred().pred();
    Tile possibleBonusCounter = currentBonus;
    unsigned int numPossibleBonusTiles = 0;
    while (possibleBonusCounter > T::_3) {
        possibleBonusCounter = possibleBonusCounter.pred();
        numPossibleBonusTiles++;
    }
    while (currentBonus >= T::_6) {
        float pThisBonus = float(1)/numPossibleBonusTiles/21;
        if (tileFinder < pThisBonus) {
            return currentBonus;
        } else {
            tileFinder -= pThisBonus;
            currentBonus = currentBonus.pred();
        }
    }
    return T::_6;
}

AboutToMoveBoard::AboutToMoveBoard(Board b, HiddenBoardState h) :
board(b),
hiddenState(h),
hasNoHint(true)
{}

HiddenBoardState HiddenBoardState::nextTurnStateWithAddedTile(Tile t) const {
    unsigned int newTurns = this->numTurns + 1;
    if (t <= T::_3 && this->onesInStack + this->twosInStack + this->threesInStack - 1 == 0) {
        return HiddenBoardState(newTurns, 4, 4, 4);
    }
    switch (t.value) {
        case T::_1: return HiddenBoardState(newTurns, this->onesInStack - 1, this->twosInStack, this->threesInStack);
        case T::_2: return HiddenBoardState(newTurns, this->onesInStack, this->twosInStack - 1, this->threesInStack);
        case T::_3: return HiddenBoardState(newTurns, this->onesInStack, this->twosInStack, this->threesInStack - 1);
        default: return HiddenBoardState(newTurns, this->onesInStack, this->twosInStack, this->threesInStack);
    }
}

AboutToMoveBoard AboutToAddTileBoard::addSpecificTile(AddedTileInfo info) const {
    auto result = AboutToMoveBoard(this->board, this->hiddenState.nextTurnStateWithAddedTile(info.newTileValue));
    result.board.set(info.newTileLocation, info.newTileValue);
    return result;
}

SearchResult AboutToMoveBoard::heuristicSearchIfMovedInDirection(Direction d, uint8_t depth, std::shared_ptr<Heuristic> h) const {
    //Assume board was moved but hasn't had tile added
    //TODO: return -INFINITY if all moves lead to death
    AboutToAddTileBoard board = this->moveWithoutAdd(d, true);
    float score = 0;
    unsigned int openNodeCount = 0;
    for (AddedTileInfoWithProbability additionInfo : board.possibleAdditionsWithProbability()) {
        AboutToMoveBoard potentialBoard = board.addSpecificTile(additionInfo.i);
        if (depth == 0) {
            score += h->evaluateWithoutDescription(potentialBoard)*additionInfo.probability;
            openNodeCount += 1;
        } else {
            vector<pair<Direction, SearchResult>> scoresForMoves;
            for (auto&& d : allDirections) {
                if (potentialBoard.isMoveValid(d)) {
                    AboutToAddTileBoard movedBoard = potentialBoard.moveWithoutAdd(d, true);
                    scoresForMoves.push_back({d, potentialBoard.heuristicSearchIfMovedInDirection(d, depth - 1, h)});
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

void Board::set(BoardIndex i, Tile t) {
    this->tiles[i.toRegularIndex()] = t;
    this->maxTile = std::max(t, this->maxTile);
    this->validMovesCacheIsValid = false;
    this->scoreCacheIsValid = false;
}

void AboutToMoveBoard::takeTurnInPlace(Direction d) {
    AboutToMoveBoard newBoard = this->moveWithAdd(d);
    this->board = newBoard.board;
    this->hiddenState = newBoard.hiddenState;
    this->generator = newBoard.generator;
    this->upcomingTile = none;
    this->hint = none;
}

AboutToAddTileBoard::AboutToAddTileBoard(Board b, EnabledIndices i, HiddenBoardState h, default_random_engine g, Hint hint):
board(b),
validIndicesForNewTile(i),
hiddenState(h),
generator(g),
upcomingTileGenerator(hint) {}

AboutToAddTileBoard AboutToMoveBoard::moveWithoutAdd(Direction d, bool exploratory) const {
    Board b = this->board;
    auto indices = b.move(d);
    return AboutToAddTileBoard(b, indices, this->hiddenState, this->generator, this->getHint(exploratory));
}

BoardIndex AboutToAddTileBoard::indexForNextTile() {
    debug(this->validIndicesForNewTile.size() == 0);
    uniform_int_distribution<unsigned long> dist(0, this->validIndicesForNewTile.size() - 1);
    unsigned long indexWithinValid = dist(this->generator);
    for (auto&& i : allIndices) {
        if (this->validIndicesForNewTile.isEnabled(i)) {
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

void AboutToMoveBoard::addTile(Direction d) {
    Tile t = this->getUpcomingTile(); //Note: getUpcomingTile changes the generator, so this must be retrieved before the index.
    AboutToAddTileBoard moved = this->moveWithoutAdd(d, true);
    BoardIndex i = moved.indexForNextTile();
    this->board.set(i, t);
    this->hiddenState = this->hiddenState.nextTurnStateWithAddedTile(t);
    this->upcomingTile = none;
    this->hint = none;
}

AboutToMoveBoard::AboutToMoveBoard(AddTile t, AboutToAddTileBoard const& other) :
board(other.board),
hiddenState(other.hiddenState),
generator(other.generator)
{
    this->addTile(t.d);
}

AboutToMoveBoard AboutToMoveBoard::moveWithAdd(Direction d) const {
    Board newBoard = this->board;
    newBoard.move(d);
    AboutToMoveBoard aboutToMove(newBoard, this->hiddenState);
    aboutToMove.hint = this->hint;
    aboutToMove.hasNoHint = false;
    aboutToMove.addTile(d);
    return aboutToMove;
}

AboutToMoveBoard::AboutToMoveBoard(AboutToMoveBoard::DifferentFuture d, AboutToMoveBoard const& other) :
hiddenState(other.hiddenState),
board(other.board),
generator(d.howDifferent),
upcomingTile(other.upcomingTile),
hint(other.hint)
{}

AboutToMoveBoard::AboutToMoveBoard(Board b,
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

AboutToMoveBoard::AboutToMoveBoard(Board b,
                       HiddenBoardState h,
                       default_random_engine hintGen,
                       cv::Mat sourceImage) :
board(b),
sourceImage(sourceImage),
generator(hintGen),
hasNoHint(false),
hint(none),
hiddenState(h) {}

Tile maxTileFromString(std::string const s) {
    vector<string> splitName;
    split(splitName, s, is_any_of("-"));
    
    deque<string> nums;
    split(nums, splitName[0], is_any_of(","));
    debug(nums.size() != 16);
    
    deque<string> nextTileHintStrings;
    split(nextTileHintStrings, splitName[1], is_any_of(","));
    debug(nextTileHintStrings.size() > 3);
    
    std::array<Tile, 16> tileList({
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY)
    });
    transform(nums.begin(), nums.end(), tileList.begin(), [](string s){
        return tileFromString(s);
    });
    
    deque<Tile> hint;
    for (auto&& s : nextTileHintStrings) {
        hint.push_back(tileFromString(s));
    }
    
    return *max_element(tileList.begin(), tileList.end());
}

AboutToMoveBoard::AboutToMoveBoard(string s) :
board(s),
hiddenState(0,4,4,4),
generator(0),
hasNoHint(false)
{
    
    vector<string> splitName;
    split(splitName, s, is_any_of("-"));
    
    deque<string> nums;
    split(nums, splitName[0], is_any_of(","));
    debug(nums.size() != 16);
    
    deque<string> nextTileHintStrings;
    split(nextTileHintStrings, splitName[1], is_any_of(","));
    debug(nextTileHintStrings.size() > 3);
    
    std::array<Tile, 16> tileList({
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY),
        Tile(T::EMPTY)
    });
    transform(nums.begin(), nums.end(), tileList.begin(), [](string s){
        return tileFromString(s);
    });
    
    deque<Tile> hint;
    for (auto&& s : nextTileHintStrings) {
        hint.push_back(tileFromString(s));
    }
    
    if (hint.size() == 1) {
        this->hint = Hint(hint[0]);
    } else if (hint.size() == 2) {
        this->hint = Hint(hint[0], hint[1]);
    } else {
        debug(hint.size() != 3);
        this->hint = Hint(hint[0], hint[1], hint[2]);
    }
}

Tile AboutToMoveBoard::genUpcomingTile() const {
    //This function contains an inlined version of possibleNextTiles, for performance reasons (to avoid creating a deque)
    
    // Due to floating point error, the sum of the probabilities for each tile may not add to 1,
    // which means if a 1 is generated for tileFinder, we get here. In this case, use 6,
    // which would have been returned had there been no floating point error.
    debug(this->hint != none);
    bool canHaveBonus = this->board.maxTile >= T::_48;
    default_random_engine genCopy = this->generator;
    uniform_real_distribution<> r(0,1);
    float tileFinder = r(genCopy);
    
    if (this->hiddenState.onesInStack > 0) {
        float pOne = nonBonusTileProbability(this->hiddenState, T::_1, canHaveBonus);
        if (tileFinder < pOne) {
            return T::_1;
        } else {
            tileFinder -= pOne;
        }
    }
    
    if (this->hiddenState.twosInStack > 0) {
        float pTwo = nonBonusTileProbability(this->hiddenState,T::_2, canHaveBonus);
        if (tileFinder < pTwo) {
            return T::_2;
        } else {
            tileFinder -= pTwo;
        }
    }
    
    if (this->hiddenState.threesInStack > 0) {
        float pThree = nonBonusTileProbability(this->hiddenState,T::_3, canHaveBonus);
        if (tileFinder < pThree) {
            return T::_3;
        } else {
            tileFinder -= pThree;
        }
    }
    
    Tile currentBonus = this->board.maxTile.pred().pred().pred();
    Tile possibleBonusCounter = currentBonus;
    unsigned int numPossibleBonusTiles = 0;
    while (possibleBonusCounter > T::_3) {
        possibleBonusCounter = possibleBonusCounter.pred();
        numPossibleBonusTiles++;
    }
    while (currentBonus >= T::_6) {
        float pThisBonus = float(1)/numPossibleBonusTiles/21;
        if (tileFinder < pThisBonus) {
            return currentBonus;
        } else {
            tileFinder -= pThisBonus;
            currentBonus = currentBonus.pred();
        }
    }
    return T::_6;
}

Tile AboutToAddTileBoard::genUpcomingTile() const {
    //This function contains an inlined version of possibleNextTiles, for performance reasons (to avoid creating a deque)
    
    // Due to floating point error, the sum of the probabilities for each tile may not add to 1,
    // which means if a 1 is generated for tileFinder, we get here. In this case, use 6,
    // which would have been returned had there been no floating point error.
    return this->upcomingTileGenerator.generateTile(this->generator, this->board.maxTile, this->hiddenState);
}

AboutToMoveBoard::AboutToMoveBoard(SetHint h, AboutToMoveBoard const& other) :
hiddenState(other.hiddenState),
board(other.board),
generator(other.generator),
upcomingTile(other.upcomingTile),
hint(make_optional(h.h)),
hasNoHint(false)
{}

AboutToAddTileBoard::AboutToAddTileBoard(SetHint h, AboutToAddTileBoard const& other) :
validIndicesForNewTile(other.validIndicesForNewTile),
hiddenState(other.hiddenState),
board(other.board),
generator(other.generator),
upcomingTileGenerator(h.h) {}

AboutToMoveBoard::AboutToMoveBoard(SetHiddenState h, AboutToMoveBoard const& other) :
hiddenState(h.h),
board(other.board),
generator(other.generator),
upcomingTile(other.upcomingTile),
hint(other.hint)
{}

long AboutToMoveBoard::countOfTile(Tile t) const {
    return count(this->board.tiles.begin(), this->board.tiles.end(), t);
}

unsigned long AboutToMoveBoard::adjacentPairCount() const {
    unsigned long count = 0;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Tile here = this->at(BoardIndex(i, j));
            if (i < 3) {
                Tile below = this->at(BoardIndex(i + 1, j));
                if (here.canMerge(below)) {
                    count++;
                }
            }
            if (j < 3) {
                Tile right = this->at(BoardIndex(i, j + 1));
                if (here.canMerge(right)) {
                    count++;
                }
            }
        }
    }
    return count;
}

unsigned long AboutToMoveBoard::adjacentOffByOneCount() const {
    unsigned long count = 0;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Tile here = this->at(BoardIndex(i, j));
            if (i < 3) {
                Tile below = this->at(BoardIndex(i + 1, j));
                if (here.canMergeOrMove(below.succ()) || here.canMergeOrMove(below.pred())) {
                    count++;
                }
            }
            if (j < 3) {
                Tile right = this->at(BoardIndex(i, j + 1));
                if (here.canMergeOrMove(right.succ()) || here.canMergeOrMove(right.pred())) {
                    count++;
                }
            }
        }
    }
    return count;
}


bool isBlocked(Tile here, optional<Tile> s1, optional<Tile> s2) {
    if (here == T::EMPTY) {
        return false;
    }
    if (s1.value_or(T::_3) < T::_3) {
        return false;
    }
    if (s2.value_or(T::_3) < T::_3) {
        return false;
    }
    if (here < s2.value_or(T::_6144) && here < s1.value_or(T::_6144)) {
        return true;
    }
    return false;
}

unsigned long AboutToMoveBoard::trappedTileCount() const {
    unsigned long count = 0;
    for (BoardIndex i : allIndices) {
        Tile here = this->at(i);
        optional<Tile> left = i.left() ? make_optional(this->at(i.left().get())) : none;
        optional<Tile> right = i.right() ? make_optional(this->at(i.right().get())) : none;
        optional<Tile> up = i.up() ? make_optional(this->at(i.up().get())) : none;
        optional<Tile> down = i.down() ? make_optional(this->at(i.down().get())) : none;
        if (isBlocked(here, up, down)) {
            count++;
        }
        if (isBlocked(here, left, right)) {
            count++; //double counting if blocked in both directions is probably a good idea...
        }
    }
    return count;
}

unsigned long AboutToMoveBoard::splitPairsOfTile(Tile t) const {
    if (this->countOfTile(t) <= 1) {
        return 0;
    }
    unsigned long count = 0;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Tile here = this->at(BoardIndex(i, j));
            if (here == t) {
                bool hasAdjacent = false;
                if (i < 3) {
                    Tile below = this->at(BoardIndex(i + 1, j));
                    if (here.canMerge(below)) {
                        hasAdjacent = true;
                    }
                }
                if (j < 3) {
                    Tile right = this->at(BoardIndex(i, j + 1));
                    if (here.canMerge(right)) {
                        hasAdjacent = true;
                    }
                }
                if (i > 0) {
                    Tile below = this->at(BoardIndex(i - 1, j));
                    if (here.canMerge(below)) {
                        hasAdjacent = true;
                    }
                }
                if (j > 0) {
                    Tile right = this->at(BoardIndex(i, j - 1));
                    if (here.canMerge(right)) {
                        hasAdjacent = true;
                    }
                }
                if (!hasAdjacent) {
                    count++;
                }
            }
        }
    }
    return count;
}

unsigned long AboutToMoveBoard::splitPairCount() const {
    unsigned long count = 0;
    for (Tile t = T::_3; t < T::_6144; t = t.succ()) {
        count += this->splitPairsOfTile(t);
    }
    return count;
}

Hint AboutToMoveBoard::getHint(bool exploratory) const {
    debug(this->hasNoHint && !exploratory);
    if (this->hint) {
        return this->hint.value();
    } else {
        array<Tile, 5> inRangeTiles({T::EMPTY,T::EMPTY,T::EMPTY,T::EMPTY,T::EMPTY});
        unsigned char tilesIndexEnd = 0;
        Tile hint1 = T::EMPTY;
        Tile hint2 = T::EMPTY;
        Tile hint3 = T::EMPTY;
        Tile actualTile = this->upcomingTile.value_or(this->genUpcomingTile());
        if (actualTile <= T::   _3) {
            return Hint(actualTile);
        } else {
            Tile maxBonusTile = this->board.maxBonusTile();
            //Add tiles that could show up
            if (actualTile.pred().pred() >= T::_6) {
                inRangeTiles[tilesIndexEnd] = actualTile.pred().pred();
                tilesIndexEnd++;
            }
            if (actualTile.pred() >= T::_6) {
                inRangeTiles[tilesIndexEnd] = actualTile.pred();
                tilesIndexEnd++;
            }
            inRangeTiles[tilesIndexEnd] = actualTile;
            tilesIndexEnd++;
            if (actualTile.succ() <= maxBonusTile) {
                inRangeTiles[tilesIndexEnd] = actualTile.succ();
                tilesIndexEnd++;
            }
            if (actualTile.succ().succ() <= maxBonusTile) {
                inRangeTiles[tilesIndexEnd] = actualTile.succ().succ();
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
        debug(hint1 > T::_6144);
        debug(hint2 > T::_6144);
        debug(hint3 > T::_6144);
        
        return result;
    }
}

Tile AboutToMoveBoard::getUpcomingTile() const {
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

bool AboutToMoveBoard::isGameOver() const {
    return this->board.validMoves().empty();
}

bool AboutToMoveBoard::canMove(Direction d) const {
    return this->board.validMoves().isEnabled(d);
}

Tile Board::maxBonusTile() const {
    return this->maxTile.pred().pred().pred();
}

bool AboutToMoveBoard::hasSameTilesAs(AboutToMoveBoard const& otherBoard) const {
    for (BoardIndex i : allIndices) {
        Tile tile = this->at(i);
        Tile otherTile = otherBoard.at(i);
        if (tile != otherTile) {
            return false;
        }
    }
    return true;
}

bool AboutToMoveBoard::hasSameTilesAs(AboutToAddTileBoard const& otherBoard) const {
    EnabledIndices excludedIndices = otherBoard.validIndicesForNewTile;
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

deque<pair<Tile, float>> AboutToAddTileBoard::possibleNextTiles() const {
    Tile maxBoardTile = this->board.maxTile;
    bool canHaveBonus = maxBoardTile >= T::_48;
    deque<pair<Tile, float>> result;
    //should be able to only add 1,2,3 if they are in the stack
    if (this->hiddenState.onesInStack > 0) {
        result.push_back({T::_1, nonBonusTileProbability(this->hiddenState, T::_1, canHaveBonus)});
    }
    if (this->hiddenState.twosInStack > 0) {
        result.push_back({T::_2, nonBonusTileProbability(this->hiddenState, T::_2, canHaveBonus)});
    }
    if (this->hiddenState.threesInStack > 0) {
        result.push_back({T::_3, nonBonusTileProbability(this->hiddenState, T::_3, canHaveBonus)});
    }
    Tile possibleBonusTileCounter = maxBoardTile.pred().pred().pred();
    unsigned int numPossibleBonusTiles = 0;
    while (possibleBonusTileCounter > T::_3) {
        possibleBonusTileCounter = possibleBonusTileCounter.pred();
        numPossibleBonusTiles++;
    }
    
    Tile availableBonusTile = maxBoardTile.pred().pred().pred();
    while (availableBonusTile > T::_3) {
        result.push_back({availableBonusTile, float(1)/numPossibleBonusTiles/21});
        availableBonusTile = availableBonusTile.pred();
    }
    return result;
}

deque<AddedTileInfoWithProbability> AboutToAddTileBoard::possibleAdditionsWithProbability() const {
    deque<AddedTileInfoWithProbability> results;
    
    deque<pair<Tile, float>> possibleNextTiles;
    if (this->upcomingTileGenerator.hasNoHint()) {
        possibleNextTiles = this->possibleNextTiles();
    } else {
        possibleNextTiles = this->upcomingTileGenerator.generateHint(this->genUpcomingTile(), this->board.maxBonusTile(), this->generator).possibleTiles();
    }
    EnabledIndices possibleNextLocations = this->validIndicesForNewTile;
    
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (BoardIndex i : allIndices) {
            if (possibleNextLocations.isEnabled(i)) {
                results.push_back({{nextTile.first, i}, nextTile.second*locationProbability});
            }
        }
    }
    return results;
}

deque<AddedTileInfo> AboutToAddTileBoard::possibleAdditions() const {
    deque<AddedTileInfo> results;
    
    deque<pair<Tile, float>> possibleNextTiles;
    if (this->upcomingTileGenerator.hasNoHint()) {
        possibleNextTiles = this->possibleNextTiles();
    } else {
        possibleNextTiles = this->upcomingTileGenerator.generateHint(this->genUpcomingTile(), this->board.maxBonusTile(), this->generator).possibleTiles();
    }
    
    for (auto&& nextTile : possibleNextTiles) {
        for (BoardIndex i : allIndices) {
            if (this->validIndicesForNewTile.isEnabled(i)) {
                results.push_back({nextTile.first, i});
            }
        }
    }
    return results;
}

ostream& operator<<(ostream &os, AboutToMoveBoard const& board) {
    if (board.hasNoHint) {
        os << "No Hint" << endl;
    } else {
        os << "Hint: " << board.getHint() << endl;
    }
    os << "Turns: " << board.hiddenState.numTurns << endl;
    os << "Score: " << board.score() << endl;
    os << "Stack: " << board.hiddenState.onesInStack << " " << board.hiddenState.twosInStack << " " << board.hiddenState.threesInStack << endl;
    os << board.board;
    return os;
}

bool Board::isMoveValid(Direction d) const {
    return this->validMoves().isEnabled(d);
}

EnabledDirections Board::validMoves() const {
    if (this->validMovesCacheIsValid) {
        return this->validMovesCache;
    }
    
    this->validMovesCache = EnabledDirections();
    for (unsigned i = 0; i < 4; i++) {
        if (this->at(BoardIndex(i, 0)).canMergeOrMove(this->at(BoardIndex(i, 1)))) {
            this->validMovesCache.set(Direction::UP);
            if (this->at(BoardIndex(i,0)) != T::EMPTY) {
                this->validMovesCache.set(Direction::DOWN);
            }
            break;
        }
        if (this->at(BoardIndex(i, 1)).canMergeOrMove(this->at(BoardIndex(i, 2)))) {
            this->validMovesCache.set(Direction::UP);
            if (this->at(BoardIndex(i,1)) != T::EMPTY) {
                this->validMovesCache.set(Direction::DOWN);
            }
            break;
        }
        if (this->at(BoardIndex(i, 2)).canMergeOrMove(this->at(BoardIndex(i, 3)))) {
            this->validMovesCache.set(Direction::UP);
            if (this->at(BoardIndex(i,2)) != T::EMPTY) {
                this->validMovesCache.set(Direction::DOWN);
            }
            break;
        }
    }
    if (!this->validMovesCache.isEnabled(Direction::DOWN)) {
        for (unsigned i = 0; i < 4; i++) {
            if (this->at(BoardIndex(i, 3)).canMergeOrMove(this->at(BoardIndex(i, 2))) ||
                this->at(BoardIndex(i, 2)).canMergeOrMove(this->at(BoardIndex(i, 1))) ||
                this->at(BoardIndex(i, 1)).canMergeOrMove(this->at(BoardIndex(i, 0)))) {
                this->validMovesCache.set(Direction::DOWN);
                break;
            }
        }
    }
    
    for (unsigned i = 0; i < 4; i++) {
        if (this->at(BoardIndex(0, i)).canMergeOrMove(this->at(BoardIndex(1, i)))) {
            this->validMovesCache.set(Direction::LEFT);
            if (this->at(BoardIndex(0, i)) != T::EMPTY) {
                this->validMovesCache.set(Direction::RIGHT);
            }
            break;
        }
        if (this->at(BoardIndex(1, i)).canMergeOrMove(this->at(BoardIndex(2, i)))) {
            this->validMovesCache.set(Direction::LEFT);
            if (this->at(BoardIndex(1, i)) != T::EMPTY) {
                this->validMovesCache.set(Direction::RIGHT);
            }
            break;
        }
        if (this->at(BoardIndex(2, i)).canMergeOrMove(this->at(BoardIndex(3, i)))) {
            this->validMovesCache.set(Direction::LEFT);
            if (this->at(BoardIndex(2, i)) != T::EMPTY) {
                this->validMovesCache.set(Direction::RIGHT);
            }
            break;
        }
    }
    if (!this->validMovesCache.isEnabled(Direction::RIGHT)) {
        for (unsigned i = 0; i < 4; i++) {
            if (this->at(BoardIndex(3, i)).canMergeOrMove(this->at(BoardIndex(2, i))) ||
                this->at(BoardIndex(2, i)).canMergeOrMove(this->at(BoardIndex(1, i))) ||
                this->at(BoardIndex(1, i)).canMergeOrMove(this->at(BoardIndex(0, i)))) {
                this->validMovesCache.set(Direction::RIGHT);
                break;
            }
        }
    }
    this->validMovesCacheIsValid = true;
    return this->validMovesCache;
}

Direction AboutToMoveBoard::randomValidMoveFromInternalGenerator() const {
    EnabledDirections moves = this->board.validMoves();
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

BoardScore AboutToMoveBoard::runRandomSimulation(unsigned int simNumber) const {
    AboutToMoveBoard copy(AboutToMoveBoard::DifferentFuture(simNumber), *this);
    copy.hasNoHint = false;
    while (!copy.isGameOver()) {
        auto d = copy.randomValidMoveFromInternalGenerator();
        try {
            MYLOG(copy);
            MYLOG(d);
            copy.takeTurnInPlace(d);
        } catch (std::exception e) {
            MYLOG(copy);
            debug();
            copy.takeTurnInPlace(d);
        }
    }
    return copy.score();
}

default_random_engine getGenerator() {
    static default_random_engine metaGenerator;
    return default_random_engine(metaGenerator());
}
