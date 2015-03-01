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
#include <map>

#include "ThreesBoard.h"

class ExpectimaxNodeBase {
public:
    ExpectimaxNodeBase(ThreesBoard const& board);
    
    virtual void fillInChildren(std::list<std::shared_ptr<ExpectimaxNodeBase>> & unfilledList) = 0;
    virtual unsigned int value() = 0;
    
    ThreesBoard board;
};

template <typename edge_type>
class ExpectimaxNode : public ExpectimaxNodeBase {
public:
    ExpectimaxNode(ThreesBoard const& board);
    
    virtual std::shared_ptr<ExpectimaxNodeBase> child(edge_type edge) = 0;
    std::map<edge_type, std::shared_ptr<ExpectimaxNodeBase>> children;
};

template<typename edge_type>
ExpectimaxNode<edge_type>::ExpectimaxNode(ThreesBoard const& board) : ExpectimaxNodeBase(board) {
    
}

#endif /* defined(__ThreesAI__ExpectimaxNode__) */
