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

#define makeHeuristic(func) (make_shared<Heuristic>([](BoardState const& b){return EvalutationWithDescription({func(b), #func});}))

class EvalutationWithDescription {
public:
    float score;
    std::string desciption;
};

class Heuristic {
    union F {
        std::function<EvalutationWithDescription(BoardState const &)> fWithDesc;
        std::function<float(BoardState const &)> fWithOutDesc;
        F() { memset( this, 0, sizeof( F ) ); }
        F(F const &f) {debug();}
        F operator=(F const& f) {debug();return F();}
        ~F() {}
    };
    bool hasDescription;
    F f;
    
public:
    ~Heuristic();
    Heuristic(std::function<float(BoardState const &)> f);
    Heuristic(std::function<EvalutationWithDescription(BoardState const &)> f);
    EvalutationWithDescription evaluateWithDescription(BoardState const &b) const;
    float evaluateWithoutDescription(BoardState const& b) const;
};

#endif /* Heuristic_hpp */
