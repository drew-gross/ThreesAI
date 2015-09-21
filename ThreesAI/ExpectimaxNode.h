//
//  ExpectimaxNode.h
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ExpectimaxNode__
#define __ThreesAI__ExpectimaxNode__

#include <stdio.h>
#include <iostream>

#include <list>
#include <map>

#include "BoardState.h"

class ExpectimaxNodeBase {
public:
    static int num_existing_nodes;
    ExpectimaxNodeBase(BoardState const& board, unsigned int depth);
    
    virtual void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList) = 0;
    virtual float value() const = 0;
    virtual void outputDot() const = 0;
    virtual void outputDotEdges(float p) const = 0;
    virtual void pruneUnreachableChildren(Hint const& nextTileHint, std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList) = 0;
    
    const BoardState board;
    
    const unsigned int depth;
};

template <typename edge_type>
class ExpectimaxNode : public ExpectimaxNodeBase {
public:
    ExpectimaxNode(BoardState const& board, unsigned int depth);
    
    virtual std::shared_ptr<const ExpectimaxNodeBase> child(edge_type const& edge) const = 0;
    std::map<edge_type, std::shared_ptr<ExpectimaxNodeBase>> children;
    bool childrenAreFilledIn() const;
    
    void outputDot() const;
};

template<typename edge_type>
ExpectimaxNode<edge_type>::ExpectimaxNode(BoardState const& board, unsigned int depth) : ExpectimaxNodeBase(board, depth) {}

template<typename edge_type>
bool ExpectimaxNode<edge_type>::childrenAreFilledIn() const {
    return !this->children.empty();
}

template<typename edge_type>
void ExpectimaxNode<edge_type>::outputDot() const {
    std::cout << "digraph {" << std::endl;
    std::cout << "\tnode [fontname=Courier]" << std::endl;
    std::cout << "\tedge [fontname=Courier]" << std::endl;
    this->outputDotEdges(NAN);
    std::cout << "}" << std::endl;
}

#endif /* defined(__ThreesAI__ExpectimaxNode__) */
