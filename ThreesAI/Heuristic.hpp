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

#define makeHeuristic(func) (Heuristic([](BoardState const& b){return EvalutationWithDescription({func(b), #func});}))

class EvalutationWithDescription {
public:
    float score;
    std::string desciption;
};

class Heuristic {
    std::function<EvalutationWithDescription(BoardState const &)> f;
public:
    Heuristic(std::function<EvalutationWithDescription(BoardState const &)> f);
    EvalutationWithDescription evaluateWithDescription(BoardState const &b) const;
    float evaluateWithoutDescription(BoardState const& b) const;
};

#endif /* Heuristic_hpp */
