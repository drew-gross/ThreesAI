//
//  Board.hpp
//  ThreesAI
//
//  Created by Drew Gross on 4/3/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#ifndef Board_hpp
#define Board_hpp

#include "EnabledIndices.hpp"
#include "Tile.hpp"
#include "Direction.h"
#include "EnabledDirections.hpp"

typedef unsigned long BoardScore;

class Board {
    
    EnabledIndices moveUp();
    EnabledIndices moveDown();
    EnabledIndices moveLeft();
    EnabledIndices moveRight();
    
    mutable bool validMovesCacheIsValid = false;
    mutable EnabledDirections validMovesCache;
    
    mutable BoardScore scoreCache;
    mutable bool scoreCacheIsValid = false;
    
    void initWithTileList(std::array<Tile, 16> tiles);
    
public:
    
    std::array<Tile, 16> tiles;
    Tile maxTile;
    
    EnabledIndices move(Direction d); //TODO: make this private
    explicit Board(std::array<Tile, 16> tiles);
    explicit Board(std::string s);
    
    void set(BoardIndex i, Tile t);
    Tile at(BoardIndex const& p) const;
    BoardScore score() const;
    bool isMoveValid(Direction d) const;
    EnabledDirections validMoves() const;
    Tile maxBonusTile() const;
};
std::ostream& operator<<(std::ostream &os, Board const& board);
#endif /* Board_hpp */
