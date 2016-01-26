//
//  Evaluators.cpp
//  ThreesAI
//
//  Created by Drew Gross on 1/25/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "Evaluators.hpp"

float countEmptyTile(BoardState const& b) {
    return b.countOfTile(Tile::EMPTY);
}

float score(BoardState const& b) {
    return b.score();
}

float countAdjacentPair(BoardState const& b) {
    return b.adjacentPairCount();
}

float countSplitPair(BoardState const& b) {
    return b.splitPairCount();
}

float simScore(BoardState const& b) {
    return b.runRandomSimulation(0);
}

float countAdjacentOffByOne(BoardState const& b) {
    return b.adjacentOffByOneCount();
}

float countTrappedTiles(BoardState const& b) {
    return b.trappedTileCount();
}

float highestIsInCorner(BoardState const& b) {
    Tile max = b.maxTile();
    if (b.at(BoardIndex(0,0)) == max ||
        b.at(BoardIndex(0,3)) == max ||
        b.at(BoardIndex(3,3)) == max ||
        b.at(BoardIndex(3,0)) == max) {
        return 1;
    }
    return 0;
}