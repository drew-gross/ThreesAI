//
//  UpcomingTileGenerator.hpp
//  ThreesAI
//
//  Created by Drew Gross on 4/12/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#ifndef UpcomingTileGenerator_hpp
#define UpcomingTileGenerator_hpp

#include "HiddenBoardState.hpp"
#include "Hint.h"

#include <boost/optional/optional.hpp>

class UpcomingTileGenerator
{
private:
    boost::optional<Tile> upcomingTile;
    boost::optional<Hint> hint;
    
    UpcomingTileGenerator() {};
    
public:
    UpcomingTileGenerator(Hint h) : hint(h) {}
    UpcomingTileGenerator(Tile t) : upcomingTile(t) {}
    
    Tile generateTile(std::default_random_engine rng, Tile maxBoardTile, HiddenBoardState hiddenState) const;
    Hint generateHint(Tile backupTile, Tile maxBonusTile, std::default_random_engine rng) const;
    bool hasNoHint() const;
};

#endif /* UpcomingTileGenerator_hpp */
