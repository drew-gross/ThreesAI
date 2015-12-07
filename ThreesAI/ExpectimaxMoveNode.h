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
    ExpectimaxMoveNode(std::shared_ptr<BoardState const> board, unsigned int depth);
    float value(std::function<float(BoardState const&)> heuristic) const;
    
    std::shared_ptr<const ExpectimaxNodeBase> child(Direction const& d) const ;
    
    std::pair<Direction, std::shared_ptr<const ExpectimaxNodeBase>> maxChild(std::function<float(BoardState const&)> heuristic) const;
    
    void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList);
    void pruneUnreachableChildren();
    void outputDotEdges(std::ostream& os, float p) const;
};

typedef std::shared_ptr<const ExpectimaxMoveNode> MoveTreePtr;

#endif /* defined(__ThreesAI__ExpectimaxMoveNode__) */
