//
//  ChanceNodeEdge.h
//  ThreesAI
//
//  Created by Drew Gross on 2/28/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ChanceNodeEdge__
#define __ThreesAI__ChanceNodeEdge__

#include "BoardState.h"

class ChanceNodeEdge {
public:
    ChanceNodeEdge(BoardState stateBeforeAdd, BoardState stateAfterAdd);
    ChanceNodeEdge(Tile newTileValue, BoardState::BoardIndex newTileLocation);
    
    Tile newTileValue;
    BoardState::BoardIndex newTileLocation;
};

//implemented for use as key in std::map
bool operator<(ChanceNodeEdge const& left, ChanceNodeEdge const& right);
bool operator==(ChanceNodeEdge const& left, ChanceNodeEdge const& right);
std::ostream& operator<<(std::ostream &os, const ChanceNodeEdge d);

#endif /* defined(__ThreesAI__ChanceNodeEdge__) */
