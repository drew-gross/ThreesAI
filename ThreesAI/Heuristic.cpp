//
//  Heuristic.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Heuristic.hpp"

Heuristic::Heuristic(std::function<EvalutationWithDescription(BoardState const&)> f) :
f(f)
{
}

EvalutationWithDescription Heuristic::evaluateWithDescription(const BoardState & b) const {
    return this->f(b);
}

float Heuristic::evaluateWithoutDescription(const BoardState & b) const {
    return this->f(b).score;
}