//
//  ManyPlayMonteCarloAI.h
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ManyPlayMonteCarloAI__
#define __ThreesAI__ManyPlayMonteCarloAI__

#include "ThreesAIBase.h"

#include <memory>

class ManyPlayMonteCarloAI : public ThreesAIBase {
    const unsigned int numPlays;
    
public:
    ManyPlayMonteCarloAI(std::shared_ptr<ThreesBoardBase> board, unsigned int numPlays);
    Direction playTurn();
};


#endif /* defined(__ThreesAI__ManyPlayMonteCarloAI__) */
