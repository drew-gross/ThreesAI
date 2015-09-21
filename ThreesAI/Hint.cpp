//
//  Hint.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/20/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Hint.h"

#include <deque>

#include "BoardState.h"

#include "Debug.h"

using namespace std;

Hint::Hint(unsigned int hint) : hint1(hint), hint2(0), hint3(0), isAnyBonus(false) {};
Hint::Hint(unsigned int hint1, unsigned int hint2, unsigned int hint3) : hint1(hint1), hint2(hint2), hint3(hint3), isAnyBonus(false) {};
Hint::Hint(Tile upcomingTile, Tile maxBonusTile, default_random_engine rng) : isAnyBonus(false), hint1(0), hint2(0), hint3(0) {
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

bool Hint::isNonBonus() const {
    return this->hint2 == 0;
}

Hint Hint::unknownBonus() {
    Hint result(0);
    result.isAnyBonus = true;
    return result;
}

ostream& operator<<(ostream &os, Hint h) {
    if (h.isAnyBonus) {
        os << "any bonus";
        return os;
    }
    os << h.hint1;
    if (h.hint2 > 0) {
        os << " " << h.hint2;
    }
    if (h.hint3 > 0) {
        os << " " << h.hint3;
    }
    return os;
}

bool Hint::exists() const {
    return !isAnyBonus && this->hint1 == 0;
}