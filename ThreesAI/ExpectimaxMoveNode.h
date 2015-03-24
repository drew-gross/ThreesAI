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
    ExpectimaxMoveNode(ThreesBoard const& board, unsigned int depth);
    float value() const;
    
    std::shared_ptr<const ExpectimaxNodeBase> child(Direction const& d) const ;
    
    std::pair<Direction, std::shared_ptr<const ExpectimaxNodeBase>> maxChild() const;
    
    void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList);
    void pruneUnreachableChildren(std::deque<unsigned int> const& nextTileHint);
    void outputDotEdges() const;
};



#endif /* defined(__ThreesAI__ExpectimaxMoveNode__) */
