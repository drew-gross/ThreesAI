//
//  ForcedHint.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "ForcedHint.hpp"

using namespace std;

ForcedHint::ForcedHint(unsigned int hint) :
isThisAnyBonus(false) {
    this->hint1 = hint;
    this->hint2 = 0;
    this->hint3 = 0;
};

ForcedHint::ForcedHint(unsigned int hint1, unsigned int hint2) :
isThisAnyBonus(false) {
    this->hint1 = hint1;
    this->hint2 = hint2;
    this->hint3 = 0;
};

ForcedHint::ForcedHint(unsigned int hint1, unsigned int hint2, unsigned int hint3) :
isThisAnyBonus(false) {
    this->hint1 = hint1;
    this->hint2 = hint2;
    this->hint3 = hint3;
};

shared_ptr<ForcedHint const> ForcedHint::unknownBonus() {
    ForcedHint * result = new ForcedHint(0);
    result->isThisAnyBonus = true;
    return shared_ptr<ForcedHint const>(result);
}

bool ForcedHint::isAnyBonus() const {
    return this->isThisAnyBonus;
}

ostream& ForcedHint::print(ostream& os) const {
    if (this->isThisAnyBonus) {
        os << "any bonus";
        return os;
    }
    os << this->hint1;
    if (this->hint2 > 0) {
        os << " " << this->hint2;
    }
    if (this->hint3 > 0) {
        os << " " << this->hint3;
    }
    return os;
}