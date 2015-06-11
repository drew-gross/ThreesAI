//
//  HumanPlayer.h
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__HumanPlayer__
#define __ThreesAI__HumanPlayer__

#include "ThreesAIBase.h"

class HumanPlayer : public ThreesAIBase {
public:
    HumanPlayer(std::unique_ptr<ThreesBoardBase>&& board);
    Direction playTurn();
};

#endif /* defined(__ThreesAI__HumanPlayer__) */
