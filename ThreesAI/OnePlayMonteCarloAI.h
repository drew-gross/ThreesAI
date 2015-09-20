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
    OnePlayMonteCarloAI(BoardState board, std::unique_ptr<BoardOutput> output);
    Direction getDirection() const;
};

#endif /* defined(__ThreesAI__ZeroDepthMonteCarloAI__) */
