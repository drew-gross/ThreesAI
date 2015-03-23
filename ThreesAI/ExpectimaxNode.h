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
    ExpectimaxNodeBase(ThreesBoard const& board, unsigned int depth);
    
    virtual void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList) = 0;
    virtual float value() const = 0;
    virtual void outputDot() const = 0;
    virtual void outputDotEdges() const = 0;
    virtual void pruneUnreachableChildren(std::deque<unsigned int> const& nextTileHint) = 0;
    
    const ThreesBoard board;
    
    const unsigned int depth;
    
    ~ExpectimaxNodeBase();
    
};

template <typename edge_type>
class ExpectimaxNode : public ExpectimaxNodeBase {
public:
    ExpectimaxNode(ThreesBoard const& board, unsigned int depth);
    
    virtual std::shared_ptr<const ExpectimaxNodeBase> child(edge_type const& edge) const = 0;
    std::map<edge_type, std::shared_ptr<ExpectimaxNodeBase>> children;
    bool childrenAreFilledIn() const;
    
    void outputDot() const;
    void outputDotEdges() const;
};

template<typename edge_type>
ExpectimaxNode<edge_type>::ExpectimaxNode(ThreesBoard const& board, unsigned int depth) : ExpectimaxNodeBase(board, depth) {
    
}

template<typename edge_type>
bool ExpectimaxNode<edge_type>::childrenAreFilledIn() const {
    return !this->children.empty() && !this->board.isGameOver();
}

template<typename edge_type>
void ExpectimaxNode<edge_type>::outputDot() const {
    std::cout << "digraph {" << std::endl;
    this->outputDotEdges();
    std::cout << "}" << std::endl;
}

template<typename edge_type>
void ExpectimaxNode<edge_type>::outputDotEdges() const {
    for (auto&& child : this->children) {
        std::cout << "\t" << long(this) << " -> " << long(child.second.get()) << " [label=\"" << child.first << "\"]" << std::endl;
    }
    for (auto&& child : this->children) {
        child.second->outputDotEdges();
    }
}

#endif /* defined(__ThreesAI__ExpectimaxNode__) */
