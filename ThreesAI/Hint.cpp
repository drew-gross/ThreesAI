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

using namespace std;

bool Hint::isNonBonus() const {
    return this->contains(Tile(T::_1)) || this->contains(Tile(T::_2)) || this->contains(Tile(T::_3));
}

deque<pair<Tile, float>> Hint::possibleTiles() const {
    float probability;
    if (this->hint3 != T::EMPTY) {
        probability = 1.0f/3.0f;
    } else if (this->hint2 != T::EMPTY) {
        probability = 1.0f/2.0f;
    } else {
        probability = 1;
    }
    deque<pair<Tile, float>> result;
    if (this->hint3 != T::EMPTY) {
        result.push_front({this->hint3, probability});
    }
    if (this->hint2 != T::EMPTY) {
        result.push_front({this->hint2, probability});
    }
    result.push_front({this->hint1, probability});
    return result;
}

Tile Hint::actualTile(default_random_engine gen) const {
    int num_options = 0;
    if (this->hint3 != T::EMPTY) {
        num_options = 3;
    } else if (this->hint2 != T::EMPTY) {
        num_options = 2;
    } else {
        num_options = 1;
    }
    uniform_int_distribution<> r(1, num_options);
    switch (r(gen)) {
        case 1: return this->hint1;
        case 2: return this->hint2;
        case 3: return this->hint3;
    }
    return T::EMPTY;
}

bool Hint::operator!=(Hint const& other) const {
    if (!other.contains(this->hint1)) {
        return true;
    }
    if (this->hint2 != other.hint2) {
        return true;
    }
    if (this->hint3 != other.hint3) {
        return true;
    }
    return false;
}

bool Hint::contains(Tile query) const {
    if (query == T::EMPTY) {
        return false;
    }
    if (query == this->hint1) {
        return true;
    }
    if (query == this->hint2) {
        return true;
    }
    if (query == this->hint3) {
        return true;
    }
    return false;
}

ostream& operator<<(ostream &os, Hint const& h) {
    return h.print(os);
}

Hint::Hint(Tile hint) :
hint1(hint),
hint2(T::EMPTY),
hint3(T::EMPTY) {};

Hint::Hint(Tile hint1, Tile hint2) :
hint1(hint1),
hint2(hint2),
hint3(T::EMPTY) {};

Hint::Hint(Tile hint1, Tile hint2, Tile hint3)  :
hint1(hint1),
hint2(hint2),
hint3(hint3) {};


ostream& Hint::print(ostream& os) const {
    os << this->hint1;
    if (this->hint2 > T::EMPTY) {
        os << " " << this->hint2;
    }
    if (this->hint3 > T::EMPTY) {
        os << " " << this->hint3;
    }
    return os;
}
