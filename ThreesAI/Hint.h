//
//  Hint.h
//  ThreesAI
//
//  Created by Drew Gross on 9/20/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef Hint_h
#define Hint_h

#include <ostream>
#include <random>
#include "Tile.hpp"

class Hint {
    unsigned int hint1;
    unsigned int hint2;
    unsigned int hint3;
    bool isAnyBonus;
public:
    explicit Hint(unsigned int hint);
    Hint(unsigned int hint1, unsigned int hint2, unsigned int hint3);
    Hint(Tile upcomingTile, Tile maxBonusTile, std::default_random_engine rng);
    static Hint unknownBonus();
    bool contains(unsigned int query) const;
    bool operator!=(Hint other) const;
    bool exists() const;
    bool isNonBonus() const;
    
    friend std::ostream& operator<<(std::ostream &os, Hint h);
};

std::ostream& operator<<(std::ostream &os, Hint h);

#endif /* Hint_h */
