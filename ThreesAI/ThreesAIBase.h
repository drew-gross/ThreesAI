//
//  ThreesAIBase.h
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ThreesAIBase__
#define __ThreesAI__ThreesAIBase__

#include "ThreesBoard.h"

class ThreesAIBase {
public:
    ThreesAIBase();
    virtual Direction playTurn() = 0;
    void playGame();
    
    ThreesBoard board;
private:
};

#endif /* defined(__ThreesAI__ThreesAIBase__) */
