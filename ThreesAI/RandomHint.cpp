//
//  RandomHint.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "RandomHint.hpp"

#include <deque>

#include "Debug.h"

using namespace std;

RandomHint::RandomHint(Tile upcomingTile, Tile maxBonusTile, default_random_engine rng) {
    deque<Tile> inRangeTiles;
    Tile actualTile = upcomingTile;
    if (actualTile <= Tile::TILE_3) {
        this->hint1 = actualTile;
        this->hint2 = Tile::EMPTY;
        this->hint3 = Tile::EMPTY;
    } else {
        //Add tiles that could show up
        if (pred(pred(actualTile)) >= Tile::TILE_6) {
            inRangeTiles.push_back(pred(pred(actualTile)));
        }
        if (pred(actualTile) >= Tile::TILE_6) {
            inRangeTiles.push_back(pred(actualTile));
        }
        inRangeTiles.push_back(actualTile);
        if (succ(actualTile) <= maxBonusTile) {
            inRangeTiles.push_back(succ(actualTile));
        }
        if (succ(succ(actualTile)) <= maxBonusTile) {
            inRangeTiles.push_back(succ(succ(actualTile)));
        }
        
        //Trim list down to 3
        while (inRangeTiles.size() > 3) {
            if (upcomingTile == *inRangeTiles.end()) {
                inRangeTiles.pop_front();
            } else if (upcomingTile == *inRangeTiles.begin()) {
                inRangeTiles.pop_back();
            } else if (uniform_int_distribution<>(0,1)(rng) == 1) {
                inRangeTiles.pop_back();
            } else {
                inRangeTiles.pop_front();
            }
        }
        
        if (inRangeTiles.size() == 3) {
            this->hint3 = inRangeTiles[2];
        }
        if (inRangeTiles.size() >= 2) {
            this->hint2 = inRangeTiles[1];
        }
        this->hint1 = inRangeTiles[0];
    }
    debug(!this->contains(upcomingTile));
    debug(this->hint1 > Tile::TILE_6144);
    debug(this->hint2 > Tile::TILE_6144);
    debug(this->hint3 > Tile::TILE_6144);
}

bool RandomHint::isAnyBonus() const {
    return false;
}

ostream& RandomHint::print(ostream& os) const {
    os << this->hint1;
    if (this->hint2 > Tile::EMPTY) {
        os << " " << this->hint2;
    }
    if (this->hint3 > Tile::EMPTY) {
        os << " " << this->hint3;
    }
    return os;
}
