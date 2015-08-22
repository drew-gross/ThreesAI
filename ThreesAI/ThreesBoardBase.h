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

class BoardInfo;

class InvalidMoveException : public std::logic_error {
public:
    InvalidMoveException() : logic_error("That move cannot be made"){};
};

class SimulatedThreesBoard;

typedef std::pair<unsigned int, unsigned int> BoardIndex;

class MoveResult {
public:
    MoveResult(unsigned int value, BoardIndex location, std::deque<unsigned int> hint, Direction d);
    unsigned int value;
    BoardIndex location;
    Direction direction;
    std::deque<unsigned int> hint;
};

class ThreesBoardBase {
public:
    typedef std::array<unsigned int, 16> Board;
    
    ThreesBoardBase(Board boardTiles, std::deque<unsigned int> initialHint);
    
    bool isGameOver() const;
    unsigned int score() const;
    std::vector<Direction> validMoves() const;
    std::deque<std::pair<unsigned int, float>> possibleNextTiles() const;
    std::vector<BoardIndex> validIndicesForNewTile(Direction movedDirection) const;
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    virtual MoveResult move(Direction d) = 0;
    virtual SimulatedThreesBoard simulatedCopy() const = 0;
    unsigned int at(BoardIndex const& i) const;
    
    unsigned int numTurns;
    
    bool hasSameTilesAs(ThreesBoardBase const& otherBoard, std::vector<BoardIndex> excludedIndices) const;
    
protected:
    bool canMove(Direction d) const;
    bool canMerge(BoardIndex const& target, BoardIndex const& here) const;
    
    MoveResult lastMove;
    
    unsigned int maxTile() const;
    
    TileStack tileStack;
    Board board;
    
    mutable bool isGameOverCache;
    mutable bool isGameOverCacheIsValid;
    
    mutable bool scoreCacheIsValid;
    mutable unsigned int scoreCache;
    friend std::ostream& operator<<(std::ostream &os, BoardInfo const& info);
    friend class ExpectimaxMoveNode;
    friend class ExpectimaxChanceNode;
};

std::ostream& operator<<(std::ostream &os, BoardInfo const& info);
std::ostream& operator<<(std::ostream &os, ThreesBoardBase const& board);

#endif /* defined(__ThreesAI__ThreesBoardBase__) */
