//
//  ThreesBoard.h
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ThreesBoard__
#define __ThreesAI__ThreesBoard__

#include <array>
#include <vector>
#include <deque>

#include "ThreesBoardBase.h"

class RealThreesBoard;

class SimulatedThreesBoard : public ThreesBoardBase {
public:
    static SimulatedThreesBoard randomBoard();
    
    SimulatedThreesBoard simulatedCopy() const;
    static SimulatedThreesBoard fromString(const std::string s);
    
    explicit SimulatedThreesBoard(Board board);
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    std::pair<unsigned int, BoardIndex> move(Direction d);
    //Returns whether or not it succeeded
    bool moveWithoutAdd(Direction d);
    
    void set(BoardIndex const& i, const unsigned int t);
    std::deque<unsigned int> nextTileHint() const;
    
    friend class RealThreesBoard;

private:
    //Adds a tile in an appropriate location given that the given direction was the most recent move. Throws if this can't be done.
    std::pair<unsigned int, BoardIndex> addTile(Direction d);
    bool tryMerge(BoardIndex const& target, BoardIndex const& other);
};

std::ostream& operator<<(std::ostream &os, const SimulatedThreesBoard::BoardIndex e);
std::ostream& operator<<(std::ostream &os, Direction d);

#endif /* defined(__ThreesAI__ThreesBoard__) */
