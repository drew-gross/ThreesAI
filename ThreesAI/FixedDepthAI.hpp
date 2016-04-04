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
    FixedDepthAI(BoardStateCPtr board, std::unique_ptr<BoardOutput> output, std::shared_ptr<Heuristic> h, uint8_t depth);
    
    void receiveState(Direction d, AboutToMoveBoard const & newState);
    void prepareDirection();
    std::shared_ptr<Heuristic> heuristic;
    
    Direction getDirection() const;
    
    const uint8_t depth;
};

#endif /* FixedDepthAI_hpp */
