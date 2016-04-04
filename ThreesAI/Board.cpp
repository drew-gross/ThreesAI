//
//  Board.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/3/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "Board.hpp"

#include <numeric>

using namespace std;

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
