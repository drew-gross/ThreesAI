//
//  main.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <iostream>
#include "ThreesBoard.h"

int main(int argc, const char * argv[]) {
    ThreesBoard board;
    std::cout << board;
    board.processInputDirection(RIGHT);
    std::cout << board;
    board.processInputDirection(RIGHT);
    std::cout << board;
    board.processInputDirection(RIGHT);
    std::cout << board;
    return 0;
}