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
    //Throws if move is invalid. Returns location and value of new tile if not.
    std::pair<unsigned int, std::pair<unsigned int, unsigned int>> move(Direction d);
    bool canMove(Direction d);
    unsigned int* at(unsigned x, unsigned y);
    unsigned int* at(std::pair<unsigned, unsigned>);
    friend std::ostream& operator<<(std::ostream &os, ThreesBoard board);
    std::deque<unsigned int> possibleUpcomingTiles();
    
    //returns a set of boards with the upcomingTile and tileStack updated, but no new tiles added to the board.
    std::vector<std::tuple<float, ThreesBoard>> possibleNextBoardStates();
    unsigned int score();
    std::vector<Direction> validMoves();
    bool isGameOver();
    std::vector<std::pair<unsigned int, unsigned int>> validIndicesForNewTile(Direction d);
    
    std::deque<unsigned int> tileStack;
    
    static unsigned int tileScore(unsigned int tileValue);
    static std::default_random_engine randomGenerator; //TODO: this should probably be stored somewhere else?
private:
    //Adds a tile in an appropriate location given that the given direction was the most recent move. Throws if this can't be done.
    std::pair<unsigned int, std::pair<unsigned int, unsigned int>> addTile(Direction d);
    bool canMerge(unsigned targetX, unsigned targetY, unsigned otherX, unsigned otherY);
    bool tryMerge(unsigned targetX, unsigned targetY, unsigned otherX, unsigned otherY);
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
