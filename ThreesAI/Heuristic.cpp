//
//  Heuristic.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Heuristic.hpp"

Heuristic::Heuristic(std::function<std::pair<float, std::string>(BoardState const&)> f, std::string name) :
evaluate(f),
name(name)
{
    this->evaluateWithoutDescription = [f](const BoardState & board){
        return f(board).first;
    };
}