//
//  ExpectimaxMoveNode.h
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ExpectimaxMoveNode__
#define __ThreesAI__ExpectimaxMoveNode__

#include <map>

#include "ExpectimaxNode.h"

class ExpectimaxChanceNode;

class ExpectimaxMoveNode : public ExpectimaxNode<Direction> {
public:
    ExpectimaxMoveNode(ThreesBoard const& board);
    unsigned int value();
    
    std::shared_ptr<ExpectimaxNodeBase> child(Direction d);    
    
    std::pair<Direction, std::shared_ptr<ExpectimaxNodeBase>> maxChild();
    
    void fillInChildren(std::list<std::shared_ptr<ExpectimaxNodeBase>> & unfilledList);
private:
};



#endif /* defined(__ThreesAI__ExpectimaxMoveNode__) */
