//
//  UpcomingTileGenerator.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/12/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "UpcomingTileGenerator.hpp"

#include <array>

using namespace std;

Hint UpcomingTileGenerator::generateHint(Tile backupTile, Tile maxBonusTile, default_random_engine rng) const {
    debug(!this->upcomingTile && !this->hint);
    if (this->hint) return this->hint.get();
    
    array<Tile, 5> inRangeTiles({
        T::EMPTY,
        T::EMPTY,
        T::EMPTY,
        T::EMPTY,
        T::EMPTY
    });
    unsigned char tilesIndexEnd = 0;
    Tile hint1 = T::EMPTY;
    Tile hint2 = T::EMPTY;
    Tile hint3 = T::EMPTY;
    Tile actualTile = this->upcomingTile.value_or(backupTile);
    if (actualTile <= T::_3) {
        return Hint(actualTile);
    } else {
        //Add tiles that could show up
        if (actualTile.pred().pred() >= Tile(T::_6)) {
            inRangeTiles[tilesIndexEnd] = actualTile.pred().pred();
            tilesIndexEnd++;
        }
        if (actualTile.pred() >= T::_6) {
            inRangeTiles[tilesIndexEnd] = actualTile.pred();
            tilesIndexEnd++;
        }
        inRangeTiles[tilesIndexEnd] = actualTile;
        tilesIndexEnd++;
        if (actualTile.succ() <= maxBonusTile) {
            inRangeTiles[tilesIndexEnd] = actualTile.succ();
            tilesIndexEnd++;
        }
        if (actualTile.succ().succ() <= maxBonusTile) {
            inRangeTiles[tilesIndexEnd] = actualTile.succ().succ();
            tilesIndexEnd++;
        }
        
        unsigned char tilesIndexBegin = 0;
        //Trim list down to 3
        while (tilesIndexEnd - tilesIndexBegin > 3) {
            if (upcomingTile == inRangeTiles[tilesIndexEnd-1]) {
                tilesIndexBegin++;
            } else if (upcomingTile == inRangeTiles[tilesIndexBegin]) {
                tilesIndexEnd--;
            } else if (uniform_int_distribution<>(0,1)(rng) == 1) {
                tilesIndexEnd--;
            } else {
                tilesIndexBegin++;
            }
        }
        
        if (tilesIndexEnd - tilesIndexBegin == 3) {
            hint3 = inRangeTiles[tilesIndexBegin + 2];
        }
        if (tilesIndexEnd - tilesIndexBegin >= 2) {
            hint2 = inRangeTiles[tilesIndexBegin + 1];
        }
        hint1 = inRangeTiles[tilesIndexBegin];
    }
    
    Hint result(hint1, hint2, hint3);
    debug(!result.contains(actualTile));
    debug(hint1 > Tile(T::_6144));
    debug(hint2 > T::_6144);
    debug(hint3 > T::_6144);
    
    return result;
}

bool UpcomingTileGenerator::hasNoHint() const {
    return !this->upcomingTile && !this->hint;
}