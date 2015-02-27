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
#include <array>
#include <vector>
#include <random>
#include <stack>
#include <exception>

typedef enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

class InvalidMoveException : public std::logic_error {
public:
    InvalidMoveException() : logic_error("That move cannot be made"){};
};

class ThreesBoard {
public:
    ThreesBoard();
    
    typedef std::pair<unsigned int, unsigned int> BoardIndex;
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    std::pair<unsigned int, BoardIndex> move(Direction d);
    bool canMove(Direction d);
    void set(BoardIndex i, unsigned int t);
    unsigned int at(BoardIndex i);
    friend std::ostream& operator<<(std::ostream &os, ThreesBoard board);
    std::deque<unsigned int> possibleUpcomingTiles();
    
    //returns a set of boards with the upcomingTile and tileStack updated, but no new tiles added to the board.
    std::vector<std::tuple<float, ThreesBoard, unsigned int>> possibleNextBoardStates(); //that unsigned int should be [unsigned int] in order to take into account possibility of a bonus tile.
    unsigned int score();
    std::vector<Direction> validMoves();
    bool isGameOver();
    std::vector<BoardIndex> validIndicesForNewTile(Direction d);
    
    std::deque<unsigned int> tileStack;
    
    static unsigned int tileScore(unsigned int tileValue);
    static std::default_random_engine randomGenerator; //TODO: this should probably be stored somewhere else?
private:
    //Adds a tile in an appropriate location given that the given direction was the most recent move. Throws if this can't be done.
    std::pair<unsigned int, BoardIndex> addTile(Direction d);
    bool canMerge(BoardIndex target, BoardIndex other);
    bool tryMerge(BoardIndex target, BoardIndex other);
    void rebuildTileStackIfNecessary();
    unsigned int getNextTile();
    unsigned int getBonusTile();
    unsigned int getMaxTile();
    unsigned int maxBonusTile();
    bool canGiveBonusTile();
    
    std::array<std::array<unsigned int, 4>, 4> board;
    unsigned int upcomingTile;
    
    static std::array<unsigned int, 12> baseStack;
};



#endif /* defined(__ThreesAI__ThreesBoard__) */
