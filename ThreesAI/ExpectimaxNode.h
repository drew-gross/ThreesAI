//
//  ExpectimaxNode.h
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ExpectimaxNode__
#define __ThreesAI__ExpectimaxNode__

#include <list>

#include "ThreesBoard.h"

class ExpectimaxNode {
public:
    ExpectimaxNode(const ThreesBoard& board);
    
    virtual void fillInChildren(std::list<std::shared_ptr<ExpectimaxNode>> unfilledList, Direction d) = 0;
    
    virtual unsigned int value() = 0;
    
    ThreesBoard board;
};

#endif /* defined(__ThreesAI__ExpectimaxNode__) */
