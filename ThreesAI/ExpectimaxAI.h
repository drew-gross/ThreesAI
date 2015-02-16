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

class ExpectimaxChanceNode;

class ExpectimaxMoveNode {
    friend class ExpectimaxAI;
public:
    unsigned int value();
    ExpectimaxChanceNode child(Direction d);
    
private:
    std::map<Direction, ExpectimaxChanceNode> children;
    std::pair<Direction, ExpectimaxChanceNode> maxChild();
    ThreesBoard board;
};

class ExpectimaxChanceNode {
public:
    unsigned int value();
    ExpectimaxMoveNode child(std::pair<unsigned int, std::pair<unsigned int, unsigned int>>);
private:
    std::map<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>, ExpectimaxMoveNode> children;
    float probability;
};

class ExpectimaxAI : public ThreesAIBase {
private:
    ExpectimaxMoveNode currentBoard;
    
public:
    
    void playTurn();
    
};

#endif /* defined(__ThreesAI__ExpectimaxAI__) */
