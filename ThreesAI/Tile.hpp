//
//  Tile.hpp
//  ThreesAI
//
//  Created by Drew Gross on 9/20/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef Tile_hpp
#define Tile_hpp

#include <string>

#include <boost/optional/optional.hpp>

enum class Tile {
    EMPTY,
    TILE_1,
    TILE_2,
    TILE_3,
    TILE_6,
    TILE_12,
    TILE_24,
    TILE_48,
    TILE_96,
    TILE_192,
    TILE_384,
    TILE_768,
    TILE_1536,
    TILE_3072,
    TILE_6144
};

Tile pred(Tile t);
Tile succ(Tile t);
unsigned int tileScore(Tile t);
Tile tileFromString(std::string s);
boost::optional<Tile> mergeResult(Tile t1, Tile t2);

std::ostream& operator<<(std::ostream &os, Tile t);

#endif /* Tile_hpp */
