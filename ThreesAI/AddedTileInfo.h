//
//  ChanceNodeEdge.h
//  ThreesAI
//
//  Created by Drew Gross on 2/28/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ChanceNodeEdge__
#define __ThreesAI__ChanceNodeEdge__

#include "Tile.hpp"
#include "BoardIndex.hpp"

class AboutToAddTileBoard;
class AboutToMoveBoard;

class AddedTileInfo {
public:
    AddedTileInfo(AboutToAddTileBoard const& stateBeforeAdd, AboutToMoveBoard const& stateAfterAdd);
    AddedTileInfo(Tile newTileValue, BoardIndex newTileLocation);
    
    Tile newTileValue;
    BoardIndex newTileLocation;
};

//implemented for use as key in std::map
bool operator<(AddedTileInfo const& left, AddedTileInfo const& right);
bool operator==(AddedTileInfo const& left, AddedTileInfo const& right);
std::ostream& operator<<(std::ostream &os, const AddedTileInfo d);

#endif /* defined(__ThreesAI__ChanceNodeEdge__) */
