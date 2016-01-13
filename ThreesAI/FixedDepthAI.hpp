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
    FixedDepthAI(BoardStateCPtr board, std::unique_ptr<BoardOutput> output, Heuristic h);
    
    void receiveState(Direction d, BoardState const & newState);
    void prepareDirection();
    Heuristic heuristic;
    float getExpectedScoreIfMovedInDirection(BoardState const& justMovedBoard, Direction d, uint8_t depth) const;
    
    Direction getDirection() const;
};

#endif /* FixedDepthAI_hpp */
