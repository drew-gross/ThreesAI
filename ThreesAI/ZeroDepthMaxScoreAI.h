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
#include "SimulatedThreesBoard.h"

class ZeroDepthMaxScoreAI : public ThreesAIBase {
    
public:
    ZeroDepthMaxScoreAI(std::unique_ptr<SimulatedThreesBoard> board);
    
    Direction playTurn();
};

#endif /* defined(__ThreesAI__ZeroDepthMaxScoreAI__) */
