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

class ExpectimaxMoveNode : public ExpectimaxNode {
public:
    ExpectimaxMoveNode(ThreesBoard const& board);
    ExpectimaxMoveNode();
    
    std::shared_ptr<ExpectimaxNode> child(Direction d);
    bool childrenAreFilledIn();
    
    unsigned int value();
    
    std::pair<Direction, std::shared_ptr<ExpectimaxNode>> maxChild();
    
    
private:
    std::map<Direction, std::shared_ptr<ExpectimaxNode>> children;
    void fillInChildren(std::list<std::shared_ptr<ExpectimaxNode>> unfilledList, Direction d);
};



#endif /* defined(__ThreesAI__ExpectimaxMoveNode__) */
