//
//  ZeroDepthMaxScoreAI.h
//  ThreesAI
//
//  Created by Drew Gross on 2/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ZeroDepthMaxScoreAI__
#define __ThreesAI__ZeroDepthMaxScoreAI__

#include "ThreesAIBase.h"

#include "Heuristic.hpp"

class DescribeReasoningAI : public ThreesAIBase {
    
public:
    DescribeReasoningAI(BoardStateCPtr board, std::unique_ptr<BoardOutput> output, std::shared_ptr<Heuristic> h);
    
    void receiveState(Direction d, AboutToMoveBoard const & newState);
    void prepareDirection();
    std::shared_ptr<Heuristic> heuristic;
    
    Direction getDirection() const;
};

#endif /* defined(__ThreesAI__ZeroDepthMaxScoreAI__) */
