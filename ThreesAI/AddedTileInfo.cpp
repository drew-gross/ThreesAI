//
//  ChanceNodeEdge.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/28/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "AddedTileInfo.h"

#include "BoardState.h"

using namespace std;

AddedTileInfo::AddedTileInfo(Tile newTileValue, BoardIndex newTileLocation) :
newTileValue(newTileValue),
newTileLocation(newTileLocation) {}

AddedTileInfo::AddedTileInfo(AboutToAddTileBoard const& stateBeforeAdd, AboutToMoveBoard const& stateAfterAdd) : newTileLocation(0,0) {
    for (auto&& index : allIndices) {
        if (stateBeforeAdd.at(index) != stateAfterAdd.at(index)) {
            this->newTileValue = stateAfterAdd.at(index);
            this->newTileLocation = index;
            return;
        }
    }
}

bool operator<(AddedTileInfo const& left, AddedTileInfo const& right) {
    if (left.newTileValue != right.newTileValue) {
        return left.newTileValue < right.newTileValue;
    }
    if (left.newTileLocation.first != right.newTileLocation.first) {
        return left.newTileLocation.first < right.newTileLocation.first;
    }
    return left.newTileLocation.second < right.newTileLocation.second;
}

bool operator==(AddedTileInfo const& left, AddedTileInfo const& right) {
    return left.newTileValue == right.newTileValue &&
           left.newTileLocation.first == right.newTileLocation.first &&
           left.newTileLocation.second == right.newTileLocation.second;
}

ostream& operator<<(ostream &os, const AddedTileInfo e){
    os << "Tile: " << e.newTileValue << endl << "Location: " << e.newTileLocation;
    return os;
}