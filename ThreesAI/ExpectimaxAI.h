//
//  ExpectimaxAI.h
//  ThreesAI
//
//  Created by Drew Gross on 2/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ExpectimaxAI__
#define __ThreesAI__ExpectimaxAI__

#include "ThreesAIBase.h"

#include <map>
#include <list>

class ExpectimaxNode {
public:
    virtual void fillInChildren(std::list<ExpectimaxNode*>& unfilledList, Direction d) = 0;
};

class ExpectimaxChanceNode;

class ExpectimaxMoveNode : ExpectimaxNode {
    friend class ExpectimaxAI;
    friend class ExpectimaxChanceNode;
public:
    ExpectimaxMoveNode(ThreesBoard const& board);
    ExpectimaxMoveNode();
    
    unsigned int value();
    ExpectimaxChanceNode& child(Direction d);
    bool childrenAreFilledIn();
    
private:
    std::map<Direction, ExpectimaxChanceNode> children;
    std::pair<Direction, ExpectimaxChanceNode> maxChild();
    void fillInChildren(std::list<ExpectimaxNode*>& unfilledList, Direction d);
    ThreesBoard board;
};

class ExpectimaxChanceNode : ExpectimaxNode {
public:
    ExpectimaxChanceNode(ThreesBoard const& board);
    ExpectimaxChanceNode(){}
    
    unsigned int value();
    std::pair<float, ExpectimaxMoveNode>& child(std::pair<unsigned int, ThreesBoard::BoardIndex>);
    ThreesBoard board;
    void fillInChildren(std::list<ExpectimaxNode*>& unfilledList, Direction d);
    bool childrenAreFilledIn();
    
private:
    std::map<std::pair<unsigned int, ThreesBoard::BoardIndex>, std::pair<float, ExpectimaxMoveNode>> children;
    float probability;
};

class ExpectimaxAI : public ThreesAIBase {
private:
    ExpectimaxMoveNode currentBoard;
    
    std::list<ExpectimaxNode*> unfilledChildren;
    
    void fillInChild();
    
public:
    ExpectimaxAI();
    
    void playTurn();
    
};

#endif /* defined(__ThreesAI__ExpectimaxAI__) */
