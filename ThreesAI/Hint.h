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
protected:
    Tile hint1;
    Tile hint2;
    Tile hint3;
    
public:
    bool contains(Tile query) const;
    bool operator!=(Hint const& other) const;
    bool isNonBonus() const;
    Tile actualTile(std::default_random_engine gen) const;
    
    friend std::ostream& operator<<(std::ostream &os, Hint const& h);

private:
    virtual std::ostream& print(std::ostream &os) const = 0;
};

std::ostream& operator<<(std::ostream &os, Hint const& h);

#endif /* Hint_h */
