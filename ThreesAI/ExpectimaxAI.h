//
//  ExpectimaxAI.h
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ExpectimaxAI__
#define __ThreesAI__ExpectimaxAI__

#include <map>
#include <list>
#include <memory>

#include "ThreesAIBase.h"
#include "ExpectimaxNode.h"
#include "ExpextimaxChanceNode.h"
#include "ExpectimaxMoveNode.h"

typedef std::function<float(BoardState const&)> Heuristic;

class ExpectimaxAI : public ThreesAIBase {
private:
    std::shared_ptr<ExpectimaxMoveNode> currentBoard;
    
    std::list<std::weak_ptr<ExpectimaxNodeBase>> unfilledChildren;
    std::shared_ptr<ExpectimaxNodeBase> nextReachableNode();
    
    void fillInChild(unsigned int n=1);
    void fillInToDepth(unsigned int n);
    
public:
    ExpectimaxAI(std::shared_ptr<BoardState const> board, std::unique_ptr<BoardOutput> output, Heuristic heuristic, unsigned int depth);
    
    std::function<float(BoardState const&)> heuristic;
    
    Direction getDirection() const;
    void prepareDirection();
    void receiveState(Direction d, BoardState const& afterMoveState);
    void setCurrentHint(Hint h);
    
    unsigned int depth;
    
};

#endif /* defined(__ThreesAI__ExpectimaxAI__) */
