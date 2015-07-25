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

#include "ThreesAIBase.h"
#include "ExpectimaxNode.h"
#include "ExpextimaxChanceNode.h"
#include "ExpectimaxMoveNode.h"

class ExpectimaxAI : public ThreesAIBase {
private:
    std::shared_ptr<ExpectimaxMoveNode> currentBoard;
    
    std::list<std::weak_ptr<ExpectimaxNodeBase>> unfilledChildren;
    std::shared_ptr<ExpectimaxNodeBase> nextReachableNode();
    
    void fillInChild(unsigned int n=1);
    
public:
    ExpectimaxAI(std::shared_ptr<ThreesBoardBase> board);
    
    Direction playTurn();
    
};

#endif /* defined(__ThreesAI__ExpectimaxAI__) */
