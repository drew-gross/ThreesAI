//
//  ForcedHint.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "ForcedHint.hpp"

using namespace std;

ForcedHint::ForcedHint(Tile hint) {
    this->hint1 = hint;
    this->hint2 = Tile::EMPTY;
    this->hint3 = Tile::EMPTY;
};

ForcedHint::ForcedHint(Tile hint1, Tile hint2) {
    this->hint1 = hint1;
    this->hint2 = hint2;
    this->hint3 = Tile::EMPTY;
};

ForcedHint::ForcedHint(Tile hint1, Tile hint2, Tile hint3) {
    this->hint1 = hint1;
    this->hint2 = hint2;
    this->hint3 = hint3;
};

shared_ptr<ForcedHint const> ForcedHint::unknownBonus() {
    ForcedHint * result = new ForcedHint(Tile::EMPTY);
    return shared_ptr<ForcedHint const>(result);
}

ostream& ForcedHint::print(ostream& os) const {
    os << this->hint1;
    if (this->hint2 > Tile::EMPTY) {
        os << " " << this->hint2;
    }
    if (this->hint3 > Tile::EMPTY) {
        os << " " << this->hint3;
    }
    return os;
}