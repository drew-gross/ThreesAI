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
        this->f.fWithDesc.~function<EvalutationWithDescription(BoardState const &)>();
    } else {
        this->f.fWithOutDesc.~function<float(BoardState const &)>();
    }
}

Heuristic::Heuristic(std::function<EvalutationWithDescription(BoardState const&)> f)
{
    this->hasDescription = true;
    this->f.fWithDesc = f;
}

Heuristic::Heuristic(std::function<float(BoardState const&)> f)
{
    this->hasDescription = false;
    this->f.fWithOutDesc = f;
}

EvalutationWithDescription Heuristic::evaluateWithDescription(const BoardState & b) const {
    if (this->hasDescription) {
        return this->f.fWithDesc(b);
    } else {
        return {this->f.fWithOutDesc(b), "undescribable"};
    }
}

float Heuristic::evaluateWithoutDescription(const BoardState & b) const {
    if (this->hasDescription) {
        return this->f.fWithDesc(b).score;
    } else {
        return this->f.fWithOutDesc(b);
    }
}