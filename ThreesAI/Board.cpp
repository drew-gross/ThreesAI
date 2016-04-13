//
//  Board.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/3/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "Board.hpp"
#include "InvalidMoveException.h"
#include <boost/algorithm/string.hpp>
#include "Debug.h"
#include "Logging.h"

#include <numeric>
#include <vector>
#include <deque>

using namespace std;
using namespace boost;

BoardScore Board::score() const {
    if (this->scoreCacheIsValid) {
        return this->scoreCache;
    } else {
        this->scoreCacheIsValid = true;
        this->scoreCache = accumulate(this->tiles.begin(), this->tiles.end(), 0, [](unsigned int acc, Tile tile){
            return acc + tileScore(tile);
        });
        return this->scoreCache;
    }
}

Tile Board::at(BoardIndex const& p) const {
    return this->tiles[p.toRegularIndex()];
}

void Board::initWithTileList(std::array<Tile, 16> tiles) {
    this->tiles = tiles;
    this->maxTile = *max_element(tiles.begin(), tiles.end());
}

Board::Board(std::array<Tile, 16> tiles) {
    this->initWithTileList(tiles);
}

Board::Board(std::string const s) {
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
    
    this->initWithTileList(tileList);
}

ostream& operator<<(ostream &os, Board const& board) {
    os << "---------------------" << endl;
    os << "|" << board.tiles[0] << "|" << board.tiles[1] << "|" << board.tiles[2] << "|" << board.tiles[3] << "|" << endl;
    os << "|" << board.tiles[4] << "|" << board.tiles[5] << "|" << board.tiles[6] << "|" << board.tiles[7] << "|" << endl;
    os << "|" << board.tiles[8] << "|" << board.tiles[9] << "|" << board.tiles[10] << "|" << board.tiles[11] << "|" << endl;
    os << "|" << board.tiles[12] << "|" << board.tiles[13] << "|" << board.tiles[14] << "|" << board.tiles[15] << "|" << endl;
    os << "---------------------";
    return os;
}


EnabledIndices Board::moveUp() {
    EnabledIndices movedColumns({});
    EnabledIndices mergedColumns({});
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 3; j++) {
            Tile target = this->at(BoardIndex(i, j));
            Tile here = this->at(BoardIndex(i, j + 1));
            if (canMergeOrMove(target, here)) {
                movedColumns.set(BoardIndex(i, 3));
            }
            auto result = mergeResult(here, target);
            if (result) {
                mergedColumns.set(BoardIndex(i, 3));
                this->set(BoardIndex(i, j), result.get());
            }
        }
    }
    
    if (__builtin_expect(movedColumns.size() == 0, 0)) {
        throw InvalidMoveException();
    }
    return mergedColumns.size() > 0 ? mergedColumns : movedColumns;
}

EnabledIndices Board::moveDown() {
    EnabledIndices movedColumns({});
    EnabledIndices mergedColumns({});
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 3; j > 0; j--) {
            Tile target = this->at(BoardIndex(i, j));
            Tile here = this->at(BoardIndex(i, j - 1));
            if (canMergeOrMove(target, here)) {
                movedColumns.set(BoardIndex(i, 0));
            }
            auto result = mergeResult(here, target);
            if (result) {
                mergedColumns.set(BoardIndex(i, 0));
                this->set(BoardIndex(i, j), result.get());
            }
        }
    }
    
    if (__builtin_expect(movedColumns.size() == 0, 0)) {
        throw InvalidMoveException();
    }
    return mergedColumns.size() > 0 ? mergedColumns : movedColumns;
}

EnabledIndices Board::moveLeft() {
    EnabledIndices movedColumns({});
    EnabledIndices mergedColumns({});
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 3; j > 0; j--) {
            Tile target = this->at(BoardIndex(j, i));
            Tile here = this->at(BoardIndex(j - 1, i));
            if (canMergeOrMove(target, here)) {
                movedColumns.set(BoardIndex(0, i));
            }
            auto result = mergeResult(here, target);
            if (result) {
                mergedColumns.set(BoardIndex(0, i));
                this->set(BoardIndex(j, i), result.get());
            }
        }
    }
    
    if (__builtin_expect(movedColumns.size() == 0, 0)) {
        throw InvalidMoveException();
    }
    return mergedColumns.size() > 0 ? mergedColumns : movedColumns;
}

EnabledIndices Board::moveRight() {
    EnabledIndices movedColumns({});
    EnabledIndices mergedColumns({});
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 3; j++) {
            Tile target = this->at(BoardIndex(i, j));
            Tile here = this->at(BoardIndex(i, j + 1));
            if (canMergeOrMove(target, here)) {
                movedColumns.set(BoardIndex(3, i));
            }
            auto result = mergeResult(here, target);
            if (result) {
                mergedColumns.set(BoardIndex(3, i));
                this->set(BoardIndex(j, i), result.get());
            }
        }
    }
    
    if (__builtin_expect(movedColumns.size() == 0, 0)) {
        throw InvalidMoveException();
    }
    return mergedColumns.size() > 0 ? mergedColumns : movedColumns;
}

EnabledIndices Board::move(Direction d) {
    switch (d) {
        case Direction::UP: return this->moveUp();
        case Direction::DOWN: return this->moveDown();
        case Direction::LEFT: return this->moveLeft();
        case Direction::RIGHT: return this->moveRight();
    }
}