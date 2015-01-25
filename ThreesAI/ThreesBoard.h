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
    friend std::ostream& operator<<(std::ostream &os, ThreesBoard board);
private:
    std::array<std::array<unsigned int, 4>, 4> board;
    void tryMerge(unsigned targetX, unsigned targetY, unsigned otherX, unsigned otherY);
};



#endif /* defined(__ThreesAI__ThreesBoard__) */
