//
//  AdaptiveDepthAI.hpp
//  ThreesAI
//
//  Created by Drew Gross on 1/16/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#ifndef AdaptiveDepthAI_hpp
#define AdaptiveDepthAI_hpp

#include "ThreesAIBase.h"

#include "Heuristic.hpp"

class AdaptiveDepthAI : public ThreesAIBase {
    
public:
    AdaptiveDepthAI(BoardStateCPtr board, std::unique_ptr<BoardOutput> output, Heuristic h, unsigned int numNodesForFurtherSearch);
    
    void receiveState(Direction d, BoardState const & newState);
    void prepareDirection();
    Heuristic heuristic;
    
    Direction getDirection() const;
    
    const unsigned int numNodesForFurtherSearch;
};

#endif /* AdaptiveDepthAI_hpp */
