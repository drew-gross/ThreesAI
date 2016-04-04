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
    HumanPlayer(std::shared_ptr<AboutToMoveBoard const> board, std::unique_ptr<BoardOutput>);
    
    void prepareDirection();
    Direction getDirection() const;
};

#endif /* defined(__ThreesAI__HumanPlayer__) */
