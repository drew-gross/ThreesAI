//
//  ChanceNodeEdge.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/28/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ChanceNodeEdge.h"

ChanceNodeEdge::ChanceNodeEdge(unsigned int newTileValue, ThreesBoard::BoardIndex newTileLocation, unsigned int newUpcomingTile) :
newTileValue(newTileValue),
newTileLocation(newTileLocation),
newUpcomingTile(newUpcomingTile) {
    
}

bool operator<(ChanceNodeEdge const& left, ChanceNodeEdge const& right) {
    if (left.newTileValue != right.newTileValue) {
        return left.newTileValue < right.newTileValue;
    }
    if (left.newUpcomingTile != right.newUpcomingTile) {
        return left.newUpcomingTile < right.newUpcomingTile;
    }
    return left.newTileLocation < right.newTileLocation;
}