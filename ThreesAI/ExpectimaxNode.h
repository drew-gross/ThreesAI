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
#include <memory>
#include <fstream>
#include <cstdlib>

#include "BoardState.h"

class ExpectimaxNodeBase {
public:
    static int num_existing_nodes;
    ExpectimaxNodeBase(std::shared_ptr<BoardState const> board, unsigned int depth);
    
    virtual void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList) = 0;
    virtual float value(std::function<float(BoardState const&)> heuristic) const = 0;
    virtual void outputDot() const = 0;
    virtual void outputDotEdges(std::ostream& os, float p) const = 0;
    virtual void pruneUnreachableChildren() = 0;
    
    std::shared_ptr<BoardState const> board; //Not const to allow for replacing with board with explicit hint once hint is known
    
    const unsigned int depth;
};

template <typename edge_type>
class ExpectimaxNode : public ExpectimaxNodeBase {
public:
    ExpectimaxNode(std::shared_ptr<BoardState const> board, unsigned int depth);
    
    virtual std::shared_ptr<const ExpectimaxNodeBase> child(edge_type const& edge) const = 0;
    std::map<edge_type, std::shared_ptr<ExpectimaxNodeBase>> children;
    bool childrenAreFilledIn() const;
    
    void outputDot() const;
};

template<typename edge_type>
ExpectimaxNode<edge_type>::ExpectimaxNode(std::shared_ptr<BoardState const> board, unsigned int depth) : ExpectimaxNodeBase(board, depth) {}

template<typename edge_type>
bool ExpectimaxNode<edge_type>::childrenAreFilledIn() const {
    return !this->children.empty();
}

template<typename edge_type>
void ExpectimaxNode<edge_type>::outputDot() const {

    std::ofstream ofs;
    ofs.open("/tmp/dot.txt", std::ofstream::out | std::ofstream::trunc);

    ofs << "digraph {" << std::endl;
    ofs << "\tnode [fontname=Courier]" << std::endl;
    ofs << "\tedge [fontname=Courier]" << std::endl;
    this->outputDotEdges(ofs, NAN);
    ofs << "}" << std::endl;
    
    ofs.close();
    
    int writer = system("/usr/local/bin/dot -Tsvg < /tmp/dot.txt |/Users/drewgross/.rvm/gems/ruby-1.9.3-p327/wrappers/bcat");
    debug(writer);
}

#endif /* defined(__ThreesAI__ExpectimaxNode__) */
