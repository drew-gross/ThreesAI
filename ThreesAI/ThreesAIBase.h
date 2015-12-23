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

#include <memory>

class ThreesAIBase {
public:
    ThreesAIBase(std::shared_ptr<BoardState const> board, std::unique_ptr<BoardOutput> output);
    void playTurn(bool printMove=false);
    void playGame(bool printMove=false, bool inspectMove=false);
    
    virtual Direction getDirection() const = 0;
    std::unique_ptr<BoardOutput> boardOutput;
    
    std::shared_ptr<BoardState const> currentState() const;
    
protected:
    virtual void receiveState(Direction d, BoardState const& newState) = 0;
    virtual void prepareDirection() = 0;
    
private:
    bool print = false;
    std::shared_ptr<BoardState const> boardState;
    
};

#endif /* defined(__ThreesAI__ThreesAIBase__) */
