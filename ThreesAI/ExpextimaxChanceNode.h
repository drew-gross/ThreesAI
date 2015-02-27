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

class ExpectimaxChanceNode : public ExpectimaxNode {
public:
    ExpectimaxChanceNode(ThreesBoard const& board);
    
    unsigned int value();
    
    std::pair<float, std::shared_ptr<ExpectimaxNode>> child(std::tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int>);
    void fillInChildren(std::list<std::shared_ptr<ExpectimaxNode>> unfilledList, Direction d);
    bool childrenAreFilledIn();
    
private:
    std::map<std::tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int>, std::pair<float, std::shared_ptr<ExpectimaxNode>>> children;
    float probability;
};


#endif /* defined(__ThreesAI__ExpextimaxChanceNode__) */
