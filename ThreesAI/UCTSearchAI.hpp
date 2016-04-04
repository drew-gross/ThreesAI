//
//  UCTSearchAI.hpp
//  ThreesAI
//
//  Created by Drew Gross on 11/9/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef UCTSearchAI_hpp
#define UCTSearchAI_hpp

#include <memory>

#include "BoardOutput.h"
#include "Direction.h"
#include "ThreesAIBase.h"

class UCTSearchAI : public ThreesAIBase {
    const unsigned int numPlays;
    
public:
    UCTSearchAI(std::shared_ptr<AboutToMoveBoard const> board, std::unique_ptr<BoardOutput> output, unsigned int numPlays);
    Direction getDirection() const;
    
    void receiveState(Direction d, AboutToMoveBoard const & newState);
    void prepareDirection();
};


#endif /* UCTSearchAI_hpp */
