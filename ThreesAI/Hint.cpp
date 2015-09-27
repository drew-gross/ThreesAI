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

bool Hint::isNonBonus() const {
    return this->contains(1) || this->contains(2) || this->contains(3);
}

bool Hint::operator!=(Hint const& other) const {
    if (this->isAnyBonus() != other.isAnyBonus()) {
        return true;
    }
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

bool Hint::contains(unsigned int query) const {
    if (query == 0) {
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
    return this->isAnyBonus() && query >= 6;
}

ostream& operator<<(ostream &os, Hint const& h) {
    return h.print(os);
}