//
//  ThreesAIBase.h
//  ThreesAI
//
//  Created by Drew Gross on 2/8/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ThreesAIBase__
#define __ThreesAI__ThreesAIBase__

#include <memory>

#include "BoardState.h"
#include "BoardOutput.h"

#include "Direction.h"

class ThreesAIBase {
public:
    ThreesAIBase(BoardState board, std::unique_ptr<BoardOutput> output);
    void playTurn();
    void playGame();
    
    virtual Direction getDirection() const = 0;
    std::unique_ptr<BoardOutput> boardOutput;
    
    BoardState currentState() const;
    
protected:
    virtual void receiveState(Direction d, BoardState const & newState) = 0;
    virtual void prepareDirection() = 0;
    
private:
    BoardState boardState;
    
};

#endif /* defined(__ThreesAI__ThreesAIBase__) */
