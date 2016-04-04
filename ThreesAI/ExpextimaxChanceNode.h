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
    ExpectimaxChanceNode(std::shared_ptr<AboutToAddTileBoard const> board, Direction d, unsigned int depth);
    float value(std::function<float(AboutToMoveBoard const&)> heuristic) const;
    
    std::shared_ptr<const ExpectimaxNodeBase> child(AddedTileInfo const& k) const;
    void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList);
    void pruneUnreachableChildren();

    std::map<AddedTileInfo, float> childrenProbabilities;
    
    Direction directionMovedToGetHere;
    
    void outputDotEdges(std::ostream& os, float p) const;
    
    std::shared_ptr<AboutToAddTileBoard const> board; //Not const to allow for replacing with board with explicit hint once hint is known
};

typedef std::shared_ptr<const ExpectimaxChanceNode> ChanceTreePtr;

#endif /* defined(__ThreesAI__ExpextimaxChanceNode__) */
