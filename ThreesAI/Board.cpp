//
//  Board.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/3/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "Board.hpp"
#include <boost/algorithm/string.hpp>
#include "Debug.h"

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
