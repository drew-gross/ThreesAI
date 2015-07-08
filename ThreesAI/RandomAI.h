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

#include <memory>

class RandomAI : public ThreesAIBase {
public:
    RandomAI(std::unique_ptr<ThreesBoardBase>&& board);
    Direction playTurn();

private:
};

#endif /* defined(__ThreesAI__RandomAI__) */
