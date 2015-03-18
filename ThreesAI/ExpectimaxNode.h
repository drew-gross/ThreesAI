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
#include <stdio.h>

#include "ThreesBoard.h"

class ExpectimaxNodeBase {
public:
    static int num_existing_nodes;
    ExpectimaxNodeBase(ThreesBoard const& board);
    
    virtual void fillInChildren(std::list<std::shared_ptr<ExpectimaxNodeBase>> & unfilledList) = 0;
    virtual unsigned int value() const = 0;
    
    const ThreesBoard board;
    
    ~ExpectimaxNodeBase();
};

template <typename edge_type>
class ExpectimaxNode : public ExpectimaxNodeBase {
public:
    ExpectimaxNode(ThreesBoard const& board);
    
    virtual std::shared_ptr<const ExpectimaxNodeBase> child(edge_type const& edge) const = 0;
    std::map<edge_type, std::shared_ptr<const ExpectimaxNodeBase>> children;
    bool childrenAreFilledIn() const;
};

template<typename edge_type>
ExpectimaxNode<edge_type>::ExpectimaxNode(ThreesBoard const& board) : ExpectimaxNodeBase(board) {
    
}

template<typename edge_type>
bool ExpectimaxNode<edge_type>::childrenAreFilledIn() const {
    return !this->children.empty();
}

#endif /* defined(__ThreesAI__ExpectimaxNode__) */
