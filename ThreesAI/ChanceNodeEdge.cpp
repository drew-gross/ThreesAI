//
//  ChanceNodeEdge.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/28/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ChanceNodeEdge.h"

using namespace std;

ChanceNodeEdge::ChanceNodeEdge(Tile newTileValue, BoardIndex newTileLocation) :
newTileValue(newTileValue),
newTileLocation(newTileLocation) {}

ChanceNodeEdge::ChanceNodeEdge(std::shared_ptr<BoardState const> stateBeforeAdd, std::shared_ptr<BoardState const> stateAfterAdd) : newTileLocation(0,0) {
    for (auto&& index : allIndices) {
        if (stateBeforeAdd->at(index) != stateAfterAdd->at(index)) {
            this->newTileValue = stateAfterAdd->at(index);
            this->newTileLocation = index;
            return;
        }
    }
}

bool operator<(ChanceNodeEdge const& left, ChanceNodeEdge const& right) {
    if (left.newTileValue != right.newTileValue) {
        return left.newTileValue < right.newTileValue;
    }
    if (left.newTileLocation.first != right.newTileLocation.first) {
        return left.newTileLocation.first < right.newTileLocation.first;
    }
    return left.newTileLocation.second < right.newTileLocation.second;
}

bool operator==(ChanceNodeEdge const& left, ChanceNodeEdge const& right) {
    return left.newTileValue == right.newTileValue &&
           left.newTileLocation.first == right.newTileLocation.first &&
           left.newTileLocation.second == right.newTileLocation.second;
}

ostream& operator<<(ostream &os, const ChanceNodeEdge e){
    os << "Tile: " << e.newTileValue << endl << "Location: " << e.newTileLocation;
    return os;
}