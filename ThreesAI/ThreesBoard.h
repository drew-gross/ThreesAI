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

#include "TileStack.h"
#include "ThreesBoardBase.h"
std::ostream& operator<<(std::ostream &os, Direction d);

class ThreesBoard : public ThreesBoardBase {
public:
    ThreesBoard();
    
    TileStack tileStack;
    
    ThreesBoard simulatedCopy();
    
    bool isGameOver();
    unsigned int score() const;
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    std::pair<unsigned int, BoardIndex> move(Direction d);
    //Returns whether or not it succeeded
    bool moveWithoutAdd(Direction d);
    bool canMove(Direction d) const;
    
    void set(BoardIndex const& i, const unsigned int t);
    unsigned int at(BoardIndex const& i) const;
    friend std::ostream& operator<<(std::ostream &os, ThreesBoard const& board);
    unsigned int maxTile() const;
    std::vector<Direction> validMoves() const;
    bool isGameOver() const;
    std::vector<BoardIndex> validIndicesForNewTile(Direction movedDirection) const;
    std::deque<unsigned int> nextTileHint() const;
    
    static unsigned int tileScore(unsigned int tileValue);

private:
    //Adds a tile in an appropriate location given that the given direction was the most recent move. Throws if this can't be done.
    std::pair<unsigned int, BoardIndex> addTile(Direction d);
    bool canMerge(BoardIndex const& target, BoardIndex const& here) const;  
    bool tryMerge(BoardIndex const& target, BoardIndex const& other);
    
    std::array<std::array<unsigned int, 4>, 4> board;
    
    mutable bool isGameOverCache;
    mutable bool isGameOverCacheIsValid;
    mutable bool scoreCacheIsValid;
    mutable unsigned int scoreCache;
};

std::ostream& operator<<(std::ostream &os, const ThreesBoard::BoardIndex e);

#endif /* defined(__ThreesAI__ThreesBoard__) */
