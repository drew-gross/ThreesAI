//
//  ChanceNodeEdge.h
//  ThreesAI
//
//  Created by Drew Gross on 2/28/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ChanceNodeEdge__
#define __ThreesAI__ChanceNodeEdge__

#include "ThreesBoard.h"

class ChanceNodeEdge {
public:
    ChanceNodeEdge(unsigned int newTileValue, ThreesBoard::BoardIndex newTileLocation, unsigned int newUpcomingTile);
    
    unsigned int newTileValue;
    ThreesBoard::BoardIndex newTileLocation;
    unsigned int newUpcomingTile;
    
};

//implemented for use as key in std::map
bool operator<(ChanceNodeEdge const& left, ChanceNodeEdge const& right);

#endif /* defined(__ThreesAI__ChanceNodeEdge__) */
