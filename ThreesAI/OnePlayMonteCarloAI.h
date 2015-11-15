//
//  ZeroDepthMonteCarloAI.h
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ZeroDepthMonteCarloAI__
#define __ThreesAI__ZeroDepthMonteCarloAI__

#include "ThreesAIBase.h"

#include <memory>

class OnePlayMonteCarloAI : public ThreesAIBase {
    
public:
    OnePlayMonteCarloAI(std::shared_ptr<BoardState const> board, std::unique_ptr<BoardOutput> output);
    Direction getDirection() const;
    
    void receiveState(Direction d, BoardState const & newState);
    void prepareDirection();
};

#endif /* defined(__ThreesAI__ZeroDepthMonteCarloAI__) */
