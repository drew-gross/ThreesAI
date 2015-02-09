//
//  RandomAI.h
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__RandomAI__
#define __ThreesAI__RandomAI__

#include "ThreesAIBase.h"

class RandomAI : ThreesAIBase {
public:
    RandomAI();
    void playTurn();
    
    ThreesBoard board;
private:
};

#endif /* defined(__ThreesAI__RandomAI__) */
