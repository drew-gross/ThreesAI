//
//  FixedDepthAI.hpp
//  ThreesAI
//
//  Created by Drew Gross on 1/10/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#ifndef FixedDepthAI_hpp
#define FixedDepthAI_hpp

#include "ThreesAIBase.h"

#include "Heuristic.hpp"

class FixedDepthAI : public ThreesAIBase {
    
public:
    FixedDepthAI(BoardStateCPtr board, std::unique_ptr<BoardOutput> output, Heuristic h, uint8_t depth);
    
    void receiveState(Direction d, BoardState const & newState);
    void prepareDirection();
    Heuristic heuristic;
    
    Direction getDirection() const;
    
    const uint8_t depth;
};

#endif /* FixedDepthAI_hpp */
