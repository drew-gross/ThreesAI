//
//  ThreesBoardBase.h
//  ThreesAI
//
//  Created by Drew Gross on 4/16/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ThreesBoardBase__
#define __ThreesAI__ThreesBoardBase__

#include <stdio.h>

#include <vector>
#include <deque>

#include "Direction.h"

class InvalidMoveException : public std::logic_error {
public:
    InvalidMoveException() : logic_error("That move cannot be made"){};
};

class SimulatedThreesBoard;

class ThreesBoardBase {
public:
    typedef std::pair<unsigned int, unsigned int> BoardIndex;
    
    ThreesBoardBase();
    
    unsigned int numTurns;
    
    virtual bool isGameOver() const = 0;
    virtual std::vector<Direction> validMoves() const = 0;
    virtual unsigned int score() const = 0;
    //Throws if move is invalid. Returns location and value of new tile if not.
    virtual std::pair<unsigned int, BoardIndex> move(Direction d) = 0;
    virtual SimulatedThreesBoard simulatedCopy() = 0;
    virtual unsigned int maxTile() const = 0;
    virtual unsigned int at(BoardIndex const& i) const = 0;
    
    virtual std::deque<unsigned int> nextTileHint() const = 0;
};

std::ostream& operator<<(std::ostream &os, ThreesBoardBase const& board);

#endif /* defined(__ThreesAI__ThreesBoardBase__) */
