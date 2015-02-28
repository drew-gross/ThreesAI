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

class ExpectimaxMoveNode;

class ExpectimaxChanceNode : public ExpectimaxNode<std::tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int>> {
public:
    ExpectimaxChanceNode(ThreesBoard const& board);
    unsigned int value();
    
    std::shared_ptr<ExpectimaxNodeBase> child(std::tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int>);
    void fillInChildren(std::list<std::shared_ptr<ExpectimaxNodeBase>> unfilledList, Direction d);
    bool childrenAreFilledIn();
    
    std::map<std::tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int>, float> childrenProbabilities;
};


#endif /* defined(__ThreesAI__ExpextimaxChanceNode__) */
