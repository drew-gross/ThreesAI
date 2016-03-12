//
//  Heuristic.hpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef Heuristic_hpp
#define Heuristic_hpp

#include "BoardState.h"

#include <functional>

#define makeHeuristic(x) (Heuristic([](BoardState const& b){return std::pair<float, std::string>(x(b), #x);}, #x))

class Heuristic {
public:
    Heuristic(std::function<std::pair<float, std::string>(BoardState const&)> f, std::string name);
    std::function<std::pair<float, std::string>(BoardState const&)> evaluate;
    std::function<float(BoardState const&)> evaluateWithoutDescription;
    std::string name;
};

#endif /* Heuristic_hpp */
