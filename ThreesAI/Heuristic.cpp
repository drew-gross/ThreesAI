//
//  Heuristic.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Heuristic.hpp"

using namespace std;

Heuristic::~Heuristic() {
    if (this->hasDescription) {
        this->f.fWithDesc.~function<EvalutationWithDescription(AboutToMoveBoard const &)>();
    } else {
        this->f.fWithOutDesc.~function<float(AboutToMoveBoard const &)>();
    }
}

Heuristic::Heuristic(std::function<EvalutationWithDescription(AboutToMoveBoard const&)> f)
{
    this->hasDescription = true;
    this->f.fWithDesc = f;
}

Heuristic::Heuristic(std::function<float(AboutToMoveBoard const&)> f)
{
    this->hasDescription = false;
    this->f.fWithOutDesc = f;
}

EvalutationWithDescription Heuristic::evaluateWithDescription(const AboutToMoveBoard & b) const {
    if (this->hasDescription) {
        return this->f.fWithDesc(b);
    } else {
        return {this->f.fWithOutDesc(b), "undescribable"};
    }
}

float Heuristic::evaluateWithoutDescription(const AboutToMoveBoard & b) const {
    if (this->hasDescription) {
        return this->f.fWithDesc(b).score;
    } else {
        return this->f.fWithOutDesc(b);
    }
}