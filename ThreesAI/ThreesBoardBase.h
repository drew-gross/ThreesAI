//
//  ThreesBoardBase.h
//  ThreesAI
//
//  Created by Drew Gross on 4/16/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ThreesBoardBase__
#define __ThreesAI__ThreesBoardBase__

#include <array>
#include <vector>
#include <deque>

#include "Direction.h"
#include "TileStack.h"

class InvalidMoveException : public std::logic_error {
public:
    InvalidMoveException() : logic_error("That move cannot be made"){};
};

class SimulatedThreesBoard;

class ThreesBoardBase {
public:
    typedef std::pair<unsigned int, unsigned int> BoardIndex;
    typedef std::array<unsigned int, 16> Board;
    
    explicit ThreesBoardBase(Board boardTiles);
    
    bool isGameOver() const;
    unsigned int score() const;
    std::vector<Direction> validMoves() const;
    std::deque<std::pair<unsigned int, float>> possibleNextTiles() const;
    std::vector<BoardIndex> validIndicesForNewTile(Direction movedDirection) const;
    
    static unsigned int tileScore(unsigned int tileValue);
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    virtual std::pair<unsigned int, BoardIndex> move(Direction d) = 0;
    virtual SimulatedThreesBoard simulatedCopy() const = 0;
    virtual std::deque<unsigned int> nextTileHint() const = 0;
    unsigned int at(BoardIndex const& i) const;
    
    unsigned int numTurns;
    
    bool hasSameTilesAs(ThreesBoardBase const& otherBoard, std::vector<ThreesBoardBase::BoardIndex> excludedIndices) const;
    
protected:
    bool canMove(Direction d) const;
    bool canMerge(BoardIndex const& target, BoardIndex const& here) const;
    
    unsigned int maxTile() const;
    
    TileStack tileStack;
    Board board;
    
    mutable bool isGameOverCache;
    mutable bool isGameOverCacheIsValid;
    
    mutable bool scoreCacheIsValid;
    mutable unsigned int scoreCache;
    
    friend std::ostream& operator<<(std::ostream &os, ThreesBoardBase const& board);
};

std::ostream& operator<<(std::ostream &os, ThreesBoardBase const& board);

#endif /* defined(__ThreesAI__ThreesBoardBase__) */
