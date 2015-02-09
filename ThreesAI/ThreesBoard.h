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

typedef enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

class ThreesBoard {
public:
    ThreesBoard();
    void processInputDirection(Direction d);
    unsigned int* at(unsigned x, unsigned y);
    unsigned int* at(std::pair<unsigned, unsigned>);
    friend std::ostream& operator<<(std::ostream &os, ThreesBoard board);
    std::deque<unsigned int> possibleUpcomingTiles();
    unsigned int score();
    
    std::stack<unsigned int> tileStack;
    
    static unsigned int tileScore(unsigned int tileValue);
    static std::default_random_engine randomGenerator; //TODO: this should probably be stored somewhere else?
private:
    void addTile(Direction d);
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
