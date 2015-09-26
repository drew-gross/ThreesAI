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
    ManyPlayMonteCarloAI(BoardState board, std::unique_ptr<BoardOutput> output, unsigned int numPlays);
    Direction getDirection() const;
    
    void receiveState(Direction d, BoardState const & newState);
    void prepareDirection();
};


#endif /* defined(__ThreesAI__ManyPlayMonteCarloAI__) */
