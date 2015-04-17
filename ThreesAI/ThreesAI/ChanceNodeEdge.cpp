//
//  ChanceNodeEdge.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/28/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ChanceNodeEdge.h"

using namespace std;

ChanceNodeEdge::ChanceNodeEdge(unsigned int newTileValue, ThreesBoard::BoardIndex newTileLocation) :
newTileValue(newTileValue),
newTileLocation(newTileLocation) {
    
}

bool operator<(ChanceNodeEdge const& left, ChanceNodeEdge const& right) {
    if (left.newTileValue != right.newTileValue) {
        return left.newTileValue < right.newTileValue;
    }
    return left.newTileLocation < right.newTileLocation;
}

bool operator==(ChanceNodeEdge const& left, ChanceNodeEdge const& right) {
    return
        left.newTileValue == right.newTileValue &&
        left.newTileLocation == right.newTileLocation;
}

ostream& operator<<(ostream &os, const ChanceNodeEdge e){
    os << "Tile: " << e.newTileValue << endl << "Location: " << e.newTileLocation;
    return os;
}