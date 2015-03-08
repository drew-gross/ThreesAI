//
//  ExpextimaxChanceNode.h
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ExpextimaxChanceNode__
#define __ThreesAI__ExpextimaxChanceNode__

#include <map>

#include "ExpectimaxNode.h"
#include "ChanceNodeEdge.h"

class ExpectimaxMoveNode;

class ExpectimaxChanceNode : public ExpectimaxNode<ChanceNodeEdge> {
public:
    ExpectimaxChanceNode(ThreesBoard const& board, Direction d);
    unsigned int value();
    
    std::shared_ptr<ExpectimaxNodeBase> child(ChanceNodeEdge const& k);
    void fillInChildren(std::list<std::shared_ptr<ExpectimaxNodeBase>> & unfilledList);

    std::map<ChanceNodeEdge, float> childrenProbabilities;
    
    Direction directionMovedToGetHere;
};

#endif /* defined(__ThreesAI__ExpextimaxChanceNode__) */
