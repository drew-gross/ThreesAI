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
#include "AddedTileInfo.h"

class ExpectimaxMoveNode;

class ExpectimaxChanceNode : public ExpectimaxNode<AddedTileInfo> {
public:
    ExpectimaxChanceNode(std::shared_ptr<BoardState const> board, Direction d, unsigned int depth);
    float value(std::function<float(BoardState const&)> heuristic) const;
    
    std::shared_ptr<const ExpectimaxNodeBase> child(AddedTileInfo const& k) const;
    void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList);
    void pruneUnreachableChildren();

    std::map<AddedTileInfo, float> childrenProbabilities;
    
    Direction directionMovedToGetHere;
    
    void outputDotEdges(std::ostream& os, float p) const;
};

typedef std::shared_ptr<const ExpectimaxChanceNode> ChanceTreePtr;

#endif /* defined(__ThreesAI__ExpextimaxChanceNode__) */
