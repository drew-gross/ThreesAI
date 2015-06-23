//
//  ThreesBoard.h
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ThreesBoard__
#define __ThreesAI__ThreesBoard__

#include <stdio.h>
#include <iostream>
#include <array>
#include <vector>
#include <random>
#include <stack>
#include <exception>
#include <unordered_map>
#include <iterator>

#include <boost/multi_array.hpp>

#include "TileStack.h"
#include "ThreesBoardBase.h"
std::ostream& operator<<(std::ostream &os, Direction d);

class SimulatedThreesBoard : public ThreesBoardBase {
public:
    static SimulatedThreesBoard randomBoard();
    
    TileStack tileStack;
    
    SimulatedThreesBoard simulatedCopy() const;
    static SimulatedThreesBoard fromString(const std::string s);
    
    explicit SimulatedThreesBoard(std::array<unsigned int, 16> board);
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    std::pair<unsigned int, BoardIndex> move(Direction d);
    //Returns whether or not it succeeded
    bool moveWithoutAdd(Direction d);
    
    void set(BoardIndex const& i, const unsigned int t);
    friend std::ostream& operator<<(std::ostream &os, SimulatedThreesBoard const& board);
    std::deque<unsigned int> nextTileHint() const;

private:
    //Adds a tile in an appropriate location given that the given direction was the most recent move. Throws if this can't be done.
    std::pair<unsigned int, BoardIndex> addTile(Direction d);
    bool tryMerge(BoardIndex const& target, BoardIndex const& other);
};

std::ostream& operator<<(std::ostream &os, const SimulatedThreesBoard::BoardIndex e);
std::ostream& operator<<(std::ostream &os, const ThreesBoardBase::ThreesBoardBase &e);

#endif /* defined(__ThreesAI__ThreesBoard__) */
