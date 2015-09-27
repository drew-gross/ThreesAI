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
    deque<unsigned int> inRangeTiles;
    unsigned int actualTile = upcomingTile;
    if (actualTile <= 3) {
        this->hint1 = actualTile;
        this->hint2 = 0;
        this->hint3 = 0;
    } else {
        //Add tiles that could show up
        if (actualTile / 4 >= 6) {
            inRangeTiles.push_back(actualTile/4);
        }
        if (actualTile / 2 >= 6) {
            inRangeTiles.push_back(actualTile/2);
        }
        inRangeTiles.push_back(actualTile);
        if (actualTile * 2 <= maxBonusTile) {
            inRangeTiles.push_back(actualTile*2);
        }
        if (actualTile * 4 <= maxBonusTile) {
            inRangeTiles.push_back(actualTile*4);
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
}

bool RandomHint::isAnyBonus() const {
    return false;
}

ostream& RandomHint::print(ostream& os) const {
    os << this->hint1;
    if (this->hint2 > 0) {
        os << " " << this->hint2;
    }
    if (this->hint3 > 0) {
        os << " " << this->hint3;
    }
    return os;
}
